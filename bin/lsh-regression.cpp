//=======================================================================
// Copyright (c) 2014 Karol Wegrzycki
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file lsh-regression.cpp
 * @brief lsh_regression binnary
 * @author Karol Wegrzycki, Andrzej Pacuk
 * @version 1.0
 * @date 2014-11-19
 */
#include "paal/regression/lsh_nearest_neighbors_regression.hpp"
#include "paal/utils/functors.hpp"
#include "paal/utils/irange.hpp"
#include "paal/utils/log_loss.hpp"
#include "paal/utils/parse_file.hpp"
#include "paal/utils/read_svm.hpp"

#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/numeric/ublas/vector_sparse.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/algorithm/fill.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/range/empty.hpp>
#include <boost/range/size.hpp>

#include <iostream>
#include <random>
#include <string>
#include <thread>
#include <utility>

using point_type = boost::numeric::ublas::compressed_vector<double>;
using paal::utils::tuple_get;

enum Metric {HAMMING, L1, L2};

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

template <typename Row = point_type, typename LshFunction>
std::vector<double> classify(std::istream &train_points_stream,
                             std::istream &test_points_stream,
                             std::size_t dimensions,
                             unsigned hash_funs_per_row,
                             unsigned passes,
                             std::size_t row_buffer_size,
                             unsigned nthread,
                             LshFunction function_generator) {

    using point_with_result_t = std::tuple<Row, int>;
    constexpr paal::utils::tuple_get<0> get_coordinates{};
    constexpr paal::utils::tuple_get<1> get_result{};
    using boost::adaptors::transformed;

    std::vector<point_with_result_t> points_buffer;

    auto &empty_points_range = points_buffer;
    // TODO: model_in model_out
    auto model = paal::make_lsh_nearest_neighbors_regression_tuple_hash(
            empty_points_range | transformed(get_coordinates),
            empty_points_range | transformed(get_result),
            passes,
            std::move(function_generator), hash_funs_per_row, nthread);

    auto &train_points = points_buffer;
    train_points.reserve(row_buffer_size);
    while (train_points_stream.good()) {
        train_points.clear();
        paal::read_svm(train_points_stream, dimensions, train_points, row_buffer_size);

        model.update(train_points | transformed(get_coordinates),
                     train_points | transformed(get_result),
                     nthread);
    }

    std::vector<double> alg_results, test_points_results;
    auto &test_points = points_buffer;
    test_points.reserve(row_buffer_size);
    while (test_points_stream.good()) {
        test_points.clear();
        paal::read_svm(test_points_stream, dimensions, test_points, row_buffer_size);

        model.test(test_points | transformed(get_coordinates),
                   std::back_inserter(alg_results));
        boost::copy(test_points | transformed(get_result),
                    std::back_inserter(test_points_results));
    }


    auto loss = paal::log_loss<double>(alg_results, test_points_results);

    std::cout << "LogLoss on test set = " << loss << std::endl;
    return alg_results;

}

int main(int argc, char** argv)
{
    unsigned passes;
    unsigned nthread;
    std::size_t dimensions;
    std::size_t row_buffer_size;
    unsigned precision;
    int seed;
    double w;
    Metric metric;
    namespace po = boost::program_options;

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
        ("test_file,t", po::value<std::string>(), "test file path (in SVM format, it doesn't metter what label says)")
        // TODO
        // ("model_in", po::value<std::string>(), "path to model, before doing any traing or testing")
        // ("model_out", po::value<std::string>(), "Write the model to this file when everything is done")
        ("result_file,o", po::value<std::string>(), "path to the file with prediction " \
                  "(float for every test in test set)")
        ("dimensions", po::value<std::size_t>(&dimensions), "number of dimensions")
        ("passes,i", po::value<unsigned>(&passes)->default_value(3), "number of iteration (default value = 3)")
        ("nthread,n", po::value<unsigned>(&nthread)->default_value(std::thread::hardware_concurrency()),
                 "number of threads (default = number of cores)")
        ("metric,m", po::value<Metric>(&metric), "Metric used for determining " \
                 "similarity bettwen objects - [HAMMING/L1/L2] (default = Hamming)")
        ("precision,b", po::value<unsigned>(&precision)->default_value(10), "Number " \
                 "of hashing function that are encoding the object")
        ("parm_w,w", po::value<double>(&w)->default_value(1000.), "Parameter w " \
                 "should be essentially bigger than radius of expected test point neighborhood")
        ("row_buffer_size", po::value<std::size_t>(&row_buffer_size)->default_value(100000),
                 "size of row buffer (default value = 100000)")
        ("seed", po::value<int>(&seed)->default_value(0), "Seed of random number generator, (default = random)")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << "\n";
        return 0;
    }

    if (vm.count("train_file") == 0 && vm.count("test_file") == 0) {
        std::cerr << "Error: Neither train_file nor test_file were set"  << std::endl;
        std::cerr << desc << std::endl;
        return 1;
    }

    if (vm.count("dimensions") == 0) {
        std::cerr << "Error: Parameter dimensions was not set"  << std::endl;
        std::cerr << desc << std::endl;
        return 1;
    }

    if (metric == HAMMING && vm.count("parm_w") > 0) {
        std::cerr << "Warning: parameter w was set, but hamming metric is used, param w is discarted" << std::endl;
    }

    //TODO:
    // if (vm.count("train_file") == 0 && vm.count("model_in") == 0) {
    //     std::cerr << "Error: If you don't set training file (train_file) you have to set "
    //          << "input model (model_in)"  << std::endl;
    //     std::cerr << desc << std::endl;
    //     return 2;
    // }

    std::ifstream train_file_stream(vm["train_file"].as<std::string>());
    std::ifstream test_file_stream(vm["test_file"].as<std::string>());

    std::vector<double> results;
    if (metric == L1) {
        auto l1 = paal::hash::l_1_hash_function_generator<>{dimensions, w, std::default_random_engine(seed)};
        results = classify(train_file_stream, test_file_stream, dimensions, precision, passes, row_buffer_size, nthread, l1);
    } else if (metric == L2) {
        auto l2 = paal::hash::l_2_hash_function_generator<>{dimensions, w, std::default_random_engine(seed)};
        results = classify(train_file_stream, test_file_stream, dimensions, precision, passes, row_buffer_size, nthread, l2);
    } else {
        auto ham = paal::hash::hamming_hash_function_generator(dimensions, std::default_random_engine(seed));
        results = classify(train_file_stream, test_file_stream, dimensions, precision, passes, row_buffer_size, nthread, ham);
    }

    //TODO: serve model_in model_out
    if (vm.count("result_file") > 0) {
        std::ofstream result_file;
        result_file.open(vm["result_file"].as<std::string>());

        for (auto d: results)
            result_file << d << "\n";

        result_file.close();
    }
}
