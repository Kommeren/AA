//=======================================================================
// Copyright (c) 2014 Andrzej Pacuk, Piotr Wygocki
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
/**
 * @file lsh_nearest_neighbors_regression.hpp
 * @brief
 * @author Andrzej Pacuk, Piotr Wygocki
 * @version 1.0
 * @date 2014-10-06
 */
#ifndef PAAL_LSH_NEAREST_NEIGHBOURS_REGRESSION_HPP
#define PAAL_LSH_NEAREST_NEIGHBOURS_REGRESSION_HPP

#define BOOST_ERROR_CODE_HEADER_ONLY

#include "paal/data_structures/thread_pool.hpp"
#include "paal/utils/accumulate_functors.hpp"
#include "paal/utils/hash.hpp"
#include "paal/utils/hash_functions.hpp"
#include "paal/utils/type_functions.hpp"

#include <boost/range/algorithm/transform.hpp>
#include <boost/range/combine.hpp>
#include <boost/range/empty.hpp>
#include <boost/range/size.hpp>
#include <boost/unordered_map.hpp>

#include <functional>
#include <iterator>
#include <thread>
#include <type_traits>
#include <vector>

namespace paal {

namespace detail {struct lightweight_tag{};}

/**
 * @brief functor represting tuple of hash functions
 */
template <typename Funs>
class hash_function_tuple {
    Funs m_hash_funs;
    using fun_t = range_to_elem_t<Funs>;

    template <typename Point>
        class apply {
            Point const & m_point;
        public:
            apply(Point const & point) :
                m_point(point) {}

            auto operator()(fun_t const &fun) const -> decltype(fun(m_point))  {
                return fun(m_point);
            }
        };

public:

    ///constructor
    hash_function_tuple(Funs funs)
        : m_hash_funs(std::move(funs)) {}

    ///operator()(), returns vector of hash values
    template <typename Point>
    auto operator()(Point && point) const {
        using hash_result_single = pure_result_of_t<fun_t(Point)>;
        std::vector<hash_result_single> values;

        values.reserve(m_hash_funs.size());
        boost::transform(m_hash_funs, std::back_inserter(values),
                apply<Point>{point});

        return values;
    }

    /// This is the lightweight version of operator().
    /// It is used when the result of this function is not stored.
    template <typename Point>
    auto operator()(Point && point, detail::lightweight_tag) const {
        return m_hash_funs | boost::adaptors::transformed(apply<Point>{point});
    }
};

/**
 * @brief
 *
 * @tparam FunctionGenerator
 */
template <typename FunctionGenerator = hash::default_hash_function_generator>
class hash_function_tuple_generator {
    using fun_t = pure_result_of_t<FunctionGenerator()>;
    using funs_t = std::vector<fun_t>;
    FunctionGenerator m_function_generator;
    unsigned m_hash_functions_per_point;
public:
    /**
     * @brief
     *
     * @param function_generator
     * @param hash_functions_per_point number of hash functions in single tuple
     */
    hash_function_tuple_generator(FunctionGenerator function_generator,
                                  unsigned hash_functions_per_point) :
        m_function_generator(std::forward<FunctionGenerator>(function_generator)),
        m_hash_functions_per_point(hash_functions_per_point) {
    }


    /**
     * @brief
     *
     * @return hash_function_tuple of m_hash_functions_per_point hash functions
     */
    //TODO change to auto, when it starts working
    hash_function_tuple<funs_t> operator()() const {
        funs_t hash_funs;
        hash_funs.reserve(m_hash_functions_per_point);
        std::generate_n(std::back_inserter(hash_funs),
                        m_hash_functions_per_point,
                        std::ref(m_function_generator));

        return hash_function_tuple<funs_t>(std::move(hash_funs));
    }
};

/**
 * @brief
 *
 * @tparam FunctionGenerator
 * @param function_generator functor generating hash functions
 * @param hash_functions_per_point number of hash functions in single tuple
 *
 * @return
 */
template <typename FunctionGenerator>
auto make_hash_function_tuple_generator(FunctionGenerator &&function_generator,
                                        unsigned hash_functions_per_point) {
    return hash_function_tuple_generator<FunctionGenerator>(
                std::forward<FunctionGenerator>(function_generator),
                hash_functions_per_point);
}

namespace detail {

    template <typename Fun, typename Point>
    auto call(Fun const & f, Point &&p, detail::lightweight_tag) {
        return f(std::forward<Point>(p));
    }

    template <typename Function, typename Point>
    auto call(hash_function_tuple<Function> const & f,
              Point &&p, detail::lightweight_tag tag) {
        return f(std::forward<Point>(p), tag);
    }
} //! detail

/**
 * @brief For each query point counts average result of train points with hash
 * equal to query point's hash, basing on Locality-sensitve hashing.
 *
 * Example: <br>
   \snippet lsh_nearest_neighbors_regression_example.cpp LSH Nearest Neighbors Regression Example
 *
 * example file is lsh_nearest_neighbors_regression_example.cpp
 *
 * @tparam HashValue return type of functions generated by LshFunctionGenerator object
 * @tparam LshFunctionGenerator type of functor which generates proper LSH functions
 * @tparam HashForHashValue hash type to be used in hash maps
 */
template <typename HashValue,
          typename LshFunctionGenerator = hash_function_tuple_generator<HashValue>,
          //TODO default value here supposed to be std::hash
          typename HashForHashValue = range_hash>
class lsh_nearest_neighbors_regression {
    LshFunctionGenerator m_lsh_function_generator;

    //TODO template param QueryResultType
    using res_accu_t = average_accumulator<>;
    using map_t = boost::unordered_map<HashValue, res_accu_t, HashForHashValue>;
    using lsh_fun_t = pure_result_of_t<LshFunctionGenerator()>;

    ///hash maps containing average result for each hash key
    std::vector<map_t> m_hash_maps;
    ///hash functions
    std::vector<lsh_fun_t> m_hashes;

    ///average result of all train points
    average_accumulator<> m_avg;

public:

    /**
     * @brief initializes model and trains model using train points and results
     *
     * @tparam TrainPoints
     * @tparam TrainResults
     * @param train_points
     * @param train_results
     * @param passes number of used LSH functions
     * @param lsh_function_generator functor generating proper LSH functions
     * @param threads_count
     */
    template <typename TrainPoints, typename TrainResults>
    lsh_nearest_neighbors_regression(
            TrainPoints &&train_points, TrainResults &&train_results,
            unsigned passes,
            LshFunctionGenerator &&lsh_function_generator,
            unsigned threads_count = std::thread::hardware_concurrency()) :
        m_lsh_function_generator(lsh_function_generator),
        m_hash_maps(passes) {

        m_hashes.reserve(passes);
        std::generate_n(std::back_inserter(m_hashes), passes,
                    std::ref(lsh_function_generator));

        update(std::forward<TrainPoints>(train_points),
               std::forward<TrainResults>(train_results),
               threads_count);
    }


    /**
     * @brief trains model
     *
     * @tparam TrainPoints
     * @tparam TrainResults
     * @param train_points
     * @param train_results
     * @param threads_count
     */
    template <typename TrainPoints, typename TrainResults>
    void update(TrainPoints &&train_points, TrainResults &&train_results,
            unsigned threads_count = std::thread::hardware_concurrency()) {

        thread_pool threads(threads_count);

        threads.post([&](){ compute_avg(train_results);});

        for (auto &&map_and_fun : boost::combine(m_hash_maps, m_hashes)) {
            auto &map = boost::get<0>(map_and_fun);
            //fun is passed by value because of efficiency reasons
            threads.post([&, fun = boost::get<1>(map_and_fun)]() {add_values(fun, map, train_points, train_results);});
        }
        threads.run();
    }

    /**
     * @brief queries model, does not heave threads_count parameter, because this is much more natural
     * to do from outside of the function
     *
     * @tparam QueryPoints
     * @tparam OutputIterator
     * @param query_points
     * @param result
     */
    template <typename QueryPoints, typename OutputIterator>
    void test(QueryPoints &&query_points, OutputIterator result) const {
        assert(!m_avg.empty());

        for (auto &&query_point : query_points) {
            average_accumulator<> avg;
            for(auto && map_and_fun : boost::combine(m_hash_maps, m_hashes)) {
                auto const &map = boost::get<0>(map_and_fun);
                auto const &fun = boost::get<1>(map_and_fun);
                auto got = map.find(detail::call(fun, query_point, detail::lightweight_tag{}),
                                    HashForHashValue{}, utils::equal_to_unspecified{});
                if (got != map.end()) {
                    avg.add_value(got->second.get_average_unsafe());
                }
            }
            *result = avg.get_average(m_avg.get_average());
            ++result;
        }
    }

private:

    ///adds values to one hash map
    template <typename Points, typename Results>
    void add_values(lsh_fun_t fun, map_t & map, Points && train_points, Results && train_results) {
        for (auto &&train_point_result : boost::combine(train_points, train_results)) {
            auto && point = boost::get<0>(train_point_result);
            auto && res = boost::get<1>(train_point_result);

            //the return value of this call might be impossible to store in the map
            auto got = map.find(call(fun, point, detail::lightweight_tag{}),
                                 HashForHashValue{}, utils::equal_to_unspecified{});
            if (got != map.end()) {
                got->second.add_value(res);
            } else {
                map[fun(point)].add_value(res);
            }

        }
    }

    ///computes average
    template <typename Results>
    void compute_avg(Results const & train_results) {
        for (auto && res :train_results) {
            m_avg.add_value(res);
        }
    }
};

/**
 * @brief this is the most general version of the make_lsh_nearest_neighbors_regression,
 *        It takes any hash function generator.
 *
 * @tparam TrainPoints
 * @tparam TrainResults
 * @tparam LshFunctionGenerator
 * @param train_points
 * @param train_results
 * @param passes number of used LSH functions
 * @param lsh_function_generator functor generating proper LSH functions
 * @param threads_count
 *
 * @return lsh_nearest_neighbors_regression model
 */

template <typename TrainPoints, typename TrainResults,
          typename LshFunctionGenerator>
auto make_lsh_nearest_neighbors_regression(
             TrainPoints &&train_points, TrainResults &&train_results,
             unsigned passes,
             LshFunctionGenerator &&lsh_function_generator,
             unsigned threads_count = std::thread::hardware_concurrency()) {
    using lsh_fun = const pure_result_of_t<LshFunctionGenerator()>;
    using hash_result = typename std::remove_reference<
        typename std::result_of<lsh_fun(
                range_to_ref_t<TrainPoints>
                )>::type
        >::type;

    return lsh_nearest_neighbors_regression<hash_result, LshFunctionGenerator>(
            std::forward<TrainPoints>(train_points),
            std::forward<TrainResults>(train_results),
            passes,
            std::forward<LshFunctionGenerator>(lsh_function_generator),
            threads_count);
}


/**
 * @brief This is the special version  of make_lsh_nearest_neighbors_regression.
 *        This version assumes that hash function is concatenation (tuple) of several hash functions.
 *        In this function user provide Function generator for the inner  functions only.
 *
 * @tparam TrainPoints
 * @tparam TrainResults
 * @tparam FunctionGenerator
 * @param train_points
 * @param train_results
 * @param passes
 * @param function_generator
 * @param hash_functions_per_point
 * @param threads_count
 *
 * @return
 */
template <typename TrainPoints, typename TrainResults,
          typename FunctionGenerator>
auto make_lsh_nearest_neighbors_regression_tuple_hash(
             TrainPoints &&train_points, TrainResults &&train_results,
             unsigned passes,
             FunctionGenerator &&function_generator,
             unsigned hash_functions_per_point,
             unsigned threads_count = std::thread::hardware_concurrency()) {

    auto tuple_lsh = paal::make_hash_function_tuple_generator(
                    std::forward<FunctionGenerator>(function_generator),
                    hash_functions_per_point);
    return make_lsh_nearest_neighbors_regression(
            std::forward<TrainPoints>(train_points),
            std::forward<TrainResults>(train_results),
            passes,
            std::move(tuple_lsh),
            threads_count);
}

} //! paal

#endif // PAAL_LSH_NEAREST_NEIGHBOURS_REGRESSION_HPP
