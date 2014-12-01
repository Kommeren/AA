#include "paal/regression/lsh_nearest_neighbors_regression.hpp"
#include "paal/utils/functors.hpp"
#include "paal/utils/irange.hpp"
#include "paal/utils/log_loss.hpp"
#include "paal/utils/parse_file.hpp"
#include "paal/utils/read_svm.hpp"

#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/vector_sparse.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/algorithm/fill.hpp>
#include <boost/range/empty.hpp>
#include <boost/range/size.hpp>

#include <iostream>
#include <random>
#include <string>



using point_type = boost::numeric::ublas::mapped_vector<double>;
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

template <typename Row, typename LshFunction>
std::vector<double> classify(const std::vector<Row> &train_points,
              const std::vector<Row> &test_points,
              std::size_t dimensions,
              int hash_funs_per_row,
              int passes,
              LshFunction function_generator) {

    assert(!boost::empty(train_points));
    constexpr paal::utils::tuple_get<0> get_coordinates{};
    constexpr paal::utils::tuple_get<1> get_result{};
    using boost::adaptors::transformed;

    const auto train_points_coordinates =
        train_points | transformed(get_coordinates);
    const auto train_points_results =
        train_points | transformed(get_result);
    const auto test_points_coordinates =
        test_points | transformed(get_coordinates);
    const auto test_points_results =
        test_points | transformed(get_result);

    // TODO: model_in model_out
    // TODO: How to use nthread?
    auto model = paal::make_lsh_nearest_neighbors_regression_tuple_hash(
            train_points_coordinates, train_points_results, passes,
            std::move(function_generator), hash_funs_per_row);

    std::vector<double> alg_results(test_points.size());
    model.test(test_points_coordinates, alg_results.begin());

    auto loss = paal::log_loss<double>(alg_results, test_points_results);

    std::cout << "LogLoss on test set = " << loss << std::endl;
    return alg_results;

}

int main(int argc, char** argv)
{
    int passes;
    int nthread;
    int precision;
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
        ("passes,i", po::value<int>(&passes)->default_value(3), "number of iteration (default value = 3)")
        ("nthread,n", po::value<int>(&nthread), "number of threads (default = number of cores)")
        ("metric,m", po::value<Metric>(&metric), "Metric used for determining " \
                 "similarity bettwen objects - [HAMMING/L1/L2] (default = Hamming)")
        ("precision,b", po::value<int>(&precision)->default_value(10), "Number " \
                 "of hashing function that are encoding the object")
        ("parm_w,w", po::value<double>(&w)->default_value(1000.), "Parameter w " \
                 "should be essentially bigger than radius of expected test point neighborhood")
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

    //TODO:
    // if (vm.count("train_file") == 0 && vm.count("model_in") == 0) {
    //     std::cerr << "Error: If you don't set training file (train_file) you have to set "
    //          << "input model (model_in)"  << std::endl;
    //     std::cerr << desc << std::endl;
    //     return 2;
    // }

    std::ifstream train_file_stream(vm["train_file"].as<std::string>());
    std::ifstream test_file_stream(vm["test_file"].as<std::string>());

    using std::get;
    auto train_points = paal::read_svm<point_type>(train_file_stream);

    auto test_points = paal::read_svm<point_type>(test_file_stream);
    auto dimensions = std::max(get<1>(train_points), get<1>(test_points));
    using paal::detail::resize_rows;
    resize_rows(get<0>(train_points), tuple_get<0>(), dimensions);
    resize_rows(get<0>(test_points), tuple_get<0>(), dimensions);

    // TODO: Nie wiem jak to zrobic ladnie.
    std::vector<double> results;
    if (metric == L1) {
        auto l1 = paal::hash::l_1_hash_function_generator<>{dimensions, w, std::default_random_engine(seed)};
        results = classify(get<0>(train_points), get<0>(test_points), dimensions, precision, passes, l1);
    } else if (metric == L2) {
        auto l2 = paal::hash::l_2_hash_function_generator<>{dimensions, w, std::default_random_engine(seed)};
        results = classify(get<0>(train_points), get<0>(test_points), dimensions, precision, passes, l2);
    } else {
        if (vm.count("param_w") > 0)
            std::cerr << "Warning: parameter w was set, but hamming mettric is used, param w is discarted" << std::endl;

        auto ham = paal::hash::hamming_hash_function_generator(dimensions, std::default_random_engine(seed));
        results = classify(get<0>(train_points), get<0>(test_points), dimensions, precision, passes, ham);
    }

    //TODO: obsluga model_in model_out
    if (vm.count("result_file") > 0) {
        std::ofstream result_file;
        result_file.open(vm["result_file"].as<std::string>());

        for (auto d: results)
            result_file << d << "\n";

        result_file.close();
    }
}
