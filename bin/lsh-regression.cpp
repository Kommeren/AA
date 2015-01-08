//=======================================================================
// Copyright (c) 2014 Karol Wegrzycki, Piotr Wygocki
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file lsh-regression.cpp
 * @brief lsh_regression binnary
 * @author Karol Wegrzycki, Andrzej Pacuk, Piotr Wygocki
 * @version 1.0
 * @date 2014-11-19
 */

#include "paal/data_structures/mapped_file.hpp"
#include "paal/regression/lsh_nearest_neighbors_regression.hpp"
#include "paal/utils/functors.hpp"
#include "paal/utils/irange.hpp"
#include "paal/utils/log_loss.hpp"
#include "paal/utils/parse_file.hpp"
#include "paal/utils/read_svm.hpp"
#include "paal/utils/singleton_iterator.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/function_output_iterator.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/numeric/ublas/vector_sparse.hpp>
#include <boost/program_options.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/range/algorithm/fill.hpp>
#include <boost/range/empty.hpp>
#include <boost/range/size.hpp>

#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <thread>
#include <utility>

using point_type = boost::numeric::ublas::compressed_vector<double>;
using paal::utils::tuple_get;
namespace po = boost::program_options;

enum Metric {NONE, HAMMING, L1, L2};

struct l1_tag{};
struct l2_tag{};
struct ham_tag{};

std::istream& operator>>(std::istream& in, Metric& metr) {
    std::string token;
    in >> token;
    boost::algorithm::to_lower(token);
    if (token == "hamming" || token == "h")
        metr = HAMMING;
    else if (token == "l1")
        metr = L1;
    else if (token == "l2")
        metr = L2;
    else
        assert(0 && "couldn't conclude metric name");
    return in;
}

struct params {
    unsigned m_passes;
    unsigned m_nthread;
    std::size_t m_dimensions;
    std::size_t m_row_buffer_size;
    unsigned m_precision;
    int m_seed;
    double m_w;
    Metric m_metric;
};

auto get_function_generator(l1_tag, params p) {
    return paal::lsh::l_1_hash_function_generator<>{p.m_dimensions, p.m_w, std::default_random_engine(p.m_seed)};
}

auto get_function_generator(l2_tag, params p) {
    return paal::lsh::l_2_hash_function_generator<>{p.m_dimensions, p.m_w, std::default_random_engine(p.m_seed)};
}

auto get_function_generator(ham_tag, params p) {
    return paal::lsh::hamming_hash_function_generator(p.m_dimensions, std::default_random_engine(p.m_seed));
}

template <typename Row = point_type, typename LshFunctionTag>
void m_main(po::variables_map vm,
            params p,
            LshFunctionTag tag) {
    using lsh_fun = paal::pure_result_of_t<
                        paal::hash_function_tuple_generator<
                                    decltype(get_function_generator(tag, p))
                                                           >()
                                          >;
    using hash_result = typename std::remove_reference<
        typename std::result_of<lsh_fun(point_type)>::type
        >::type;
    using model_t = paal::lsh_nearest_neighbors_regression<hash_result, lsh_fun>;
    using point_with_result_t = std::tuple<Row, int>;
    using boost::adaptors::transformed;
    constexpr paal::utils::tuple_get<0> get_coordinates{};
    constexpr paal::utils::tuple_get<1> get_result{};
    std::vector<point_with_result_t> points_buffer;

    model_t model;

    if (vm.count("model_in")) {
        Metric metric;
        std::ifstream ifs(vm["model_in"].as<std::string>());
        boost::archive::binary_iarchive ia(ifs);
        ia >> metric;
        ia >> model;
        assert(metric == p.m_metric);
    } else {
        model = paal::make_lsh_nearest_neighbors_regression_tuple_hash(
                        points_buffer | transformed(get_coordinates),
                        points_buffer | transformed(get_result),
                        p.m_passes,
                        get_function_generator(tag, p) , p.m_precision, p.m_nthread);
    }

    if (vm.count("train_file")) {
        std::ifstream train_file_stream(vm["train_file"].as<std::string>());


        points_buffer.reserve(p.m_row_buffer_size);
        while (train_file_stream.good()) {
            points_buffer.clear();
            paal::read_svm(train_file_stream, p.m_dimensions, points_buffer, p.m_row_buffer_size);

            model.update(points_buffer | transformed(get_coordinates),
                         points_buffer | transformed(get_result),
                         p.m_nthread);
        }
    }

    if (vm.count("test_file")) {
        // Our algorithm returns range<tuple<Prediction,RealValue>>
        constexpr paal::utils::tuple_get<0> get_prediction{};
        constexpr paal::utils::tuple_get<1> get_real_value{};
        std::string const test_path = vm["test_file"].as<std::string>();

        // Lambda - for given line return tuple<LshPrediction,RealValue>>
        auto line_tester = [&](std::string const & line) {
            double test_result;

            std::stringstream row_stream(line);
            paal::svm_row<Row, int> row{p.m_dimensions};
            row_stream >> row;

            auto test_point_iterator = paal::utils::make_singleton_range(row.get_coordinates());
            auto output_iterator = boost::make_function_output_iterator([&](double x){test_result = x;});

            model.test(test_point_iterator, output_iterator);

            return std::make_tuple(test_result,row.get_result());
        };

        // compute results using n threads and our lambda
        auto test_results = paal::data_structures::for_each_line(line_tester, test_path, p.m_nthread);

        auto loss = paal::log_loss<double>(test_results | transformed(get_prediction),
                                           test_results | transformed(get_real_value));

        std::cout << "logloss on test set = " << loss << std::endl;

        if (vm.count("result_file") > 0) {
            std::ofstream result_file(vm["result_file"].as<std::string>());

            for (auto d: test_results | transformed(get_prediction)) {
                result_file << d << "\n";
            }
        }
    }

    if (vm.count("model_out")) {
        std::ofstream ofs(vm["model_out"].as<std::string>());
        boost::archive::binary_oarchive oa(ofs);
        oa << p.m_metric;
        oa << model;
    }
}

int main(int argc, char** argv)
{
    params p{};

    po::options_description desc("LSH nearest neighbors regression - \n"\
            "suite for fast machine learning KNN algorithm which is using "\
            "locality sensitive hashing functions\n\nUsage:\n"\
            "This command will train on train file and output the predictions in test_file:\n"
            "\tlsh-regression --train_file path_to_train_file --test_file path_to_test_file\n\n"\
            "If you want to use L1 metric, with 7 passes and 10 threads, and save model\n"\
            "you can use following command:\n"
            "\tlsh-regression -d train.svm -i 7 -n 10 -m L1 --model_out model.lsh\n\n"\
            "Then if you want to use this model to make a prediction to result_file:\n"\
            "\tlsh-regression -t test.svm --model_in model.lsh -o results.txt\n\n"
            "Options description");

    desc.add_options()
        ("help,h", "help message")
        ("train_file,d", po::value<std::string>(), "training file path (in SVM format)")
        ("test_file,t", po::value<std::string>(), "test file path (in SVM format, it doesn't matter what label says)")
        ("model_in", po::value<std::string>(), "path to model, before doing any training or testing")
        ("model_out", po::value<std::string>(), "Write the model to this file when everything is done")
        ("result_file,o", po::value<std::string>(), "path to the file with prediction " \
                  "(float for every test in test set)")
        ("dimensions", po::value<std::size_t>(&p.m_dimensions), "number of dimensions")
        ("passes,i", po::value<unsigned>(&p.m_passes)->default_value(3), "number of iteration (default value = 3)")
        ("nthread,n", po::value<unsigned>(&p.m_nthread)->default_value(std::thread::hardware_concurrency()),
                 "number of threads (default = number of cores)")
        ("metric,m", po::value<Metric>(&p.m_metric), "Metric used for determining " \
                 "similarity between objects - [HAMMING/L1/L2] (default = Hamming)")
        ("precision,b", po::value<unsigned>(&p.m_precision)->default_value(10), "Number " \
                 "of hashing function that are encoding the object")
        ("parm_w,w", po::value<double>(&p.m_w)->default_value(1000.), "Parameter w " \
                 "should be essentially bigger than radius of expected test point neighborhood")
        ("row_buffer_size", po::value<std::size_t>(&p.m_row_buffer_size)->default_value(100000),
                 "size of row buffer (default value = 100000)")
        ("seed", po::value<int>(&p.m_seed)->default_value(0), "Seed of random number generator, (default = random)")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << "\n";
        return EXIT_SUCCESS;
    }

    if (vm.count("train_file") == 0 && vm.count("test_file") == 0) {
        std::cerr << "Error: Neither train_file nor test_file were set"  << std::endl;
        std::cerr << desc << std::endl;
        return EXIT_FAILURE;
    }

    if (vm.count("dimensions") == 0 && vm.count("model_in") == 0) {
        std::cerr << "Error: Parameter dimensions was not set"  << std::endl;
        std::cerr << desc << std::endl;
        return EXIT_FAILURE;
    }


    if (vm.count("train_file") == 0 && vm.count("model_in") == 0) {
        std::cerr << "Error: If you don't set training file (train_file) you have to set "
             << "input model (model_in)"  << std::endl;
        std::cerr << desc << std::endl;
        return EXIT_FAILURE;
    }

    if (vm.count("model_in")) {
        Metric m;
        std::ifstream ifs(vm["model_in"].as<std::string>());
        boost::archive::binary_iarchive ia(ifs);

        ia >> m;
        assert(m != NONE);

        if (p.m_metric != NONE) {
            if (p.m_metric == m) {
                std::cerr << "Warning: if input model is specified one does not have to specify the metric"  << std::endl;
            } else {
                std::cerr << "Warning: the specified metric is ignored, because it differs from the input model metric"  << std::endl;
            }
        }
        auto ignored = [&](std::string const & param, std::string const & param_display) {
            if (vm.count(param) > 0) {
                std::cerr << "Warning: parameter " + param_display + " was set, but model_in is used, param " + param_display + " is discarded" << std::endl;
            }
        };
        ignored("parm_w", "w");
        ignored("precision", "precision");
        ignored("seed", "seed");
        ignored("passes", "passes");

        p.m_metric = m;
    } else {
        if (p.m_metric == NONE) p.m_metric = HAMMING;
        if (p.m_metric == HAMMING && vm.count("parm_w") > 0) {
            std::cerr << "Warning: parameter w was set, but hamming metric is used, param w is discarded" << std::endl;
        }
    }

    switch (p.m_metric) {
        case L1: m_main(vm, p, l1_tag{});
            break;

        case L2: m_main(vm, p, l2_tag{});
            break;

        case HAMMING: m_main(vm, p, ham_tag{});
            break;

        default:
            assert(false);
    }
    return EXIT_SUCCESS;
}
