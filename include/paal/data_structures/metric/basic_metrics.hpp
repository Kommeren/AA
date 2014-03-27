/**
 * @file basic_metrics.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-15
 */
#ifndef BASIC_METRICS_HPP
#define BASIC_METRICS_HPP

#include <array>
#include <boost/multi_array.hpp>
#include <boost/range/iterator_range.hpp>
#include "metric_traits.hpp"


namespace paal {
namespace data_structures {

/**
 * @class rectangle_array_metric
 * @brief \ref metric implementation on 2 dimensional array
 *        distance calls on this metric are valid opnly when x < N and y  < M
 *        (N and M given in the constructor)
 *        when we know that only certain calls occurs it might be worthwhile to use this metric
 *
 * @tparam DistanceTypeParam
 */
template <typename DistanceTypeParam>
class rectangle_array_metric {
    public:
        typedef DistanceTypeParam DistanceType;
        typedef int VertexType;
        /**
         * @brief constructor
         *
         * @param N
         * @param M
         */
        rectangle_array_metric(int N = 0, int M = 0) : m_matrix(boost::extents[N][M]) { }

        /**
         * @brief operator(), valid only when v < N and w < M
         *
         * @param v
         * @param w
         *
         * @return
         */
        DistanceType operator()(const VertexType & v, const VertexType & w) const {
            return m_matrix[v][w];
        }

        /**
         * @brief operator(), valid only when v < N and w < M, nonconst version
         *
         * @param v
         * @param w
         *
         * @return
         */
        DistanceType & operator()(const VertexType & v, const VertexType & w) {
            return m_matrix[v][w];
        }


        /**
         * @brief constructor from another metric
         *
         * @tparam OtherMetrics
         * @tparam XIterator
         * @tparam YIterator
         * @param other
         * @param xBegin
         * @param xEnd
         * @param yBegin
         * @param yEnd
         */
        template <typename OtherMetrics, typename XIterator, typename YIterator>
        rectangle_array_metric(const OtherMetrics& other,
                   XIterator xBegin, XIterator xEnd,
                   YIterator yBegin, YIterator yEnd) :
                        rectangle_array_metric(std::distance(xBegin, xEnd),
                                              std::distance(yBegin, yEnd)) {
            int i = 0;
            for (auto v: boost::make_iterator_range(xBegin, xEnd)) {
                int j = 0;
                for (auto w: boost::make_iterator_range(yBegin, yEnd)) {
                    this->m_matrix[i][j] = other(v, w);
                    ++j;
                }
                ++i;
            }
        }

        /**
         * @brief operator=
         *
         * @param am
         *
         * @return
         */
        rectangle_array_metric & operator=(const rectangle_array_metric & am) {
            auto shape = am.m_matrix.shape();
            std::vector<std::size_t> dim(shape, shape + DIM_NR);
            m_matrix.resize(dim);
            m_matrix = am.m_matrix;
            return *this;
        }

    protected:
        /**
         * @brief dimention of multi array
         */
        static const int DIM_NR = 2;
        typedef boost::multi_array<DistanceType, DIM_NR> matrix_type;
        ///matrix with data
        matrix_type m_matrix;
};


/**
 * @brief this metric is rectangle_array_metric with N == M.
 *
 * @tparam DistanceTypeParam
 */
template <typename DistanceTypeParam>
class array_metric : public rectangle_array_metric<DistanceTypeParam> {
    typedef rectangle_array_metric<DistanceTypeParam> base;
public:
    /**
     * @brief constructor
     *
     * @param N
     */
    array_metric(int N = 0) : base(N, N) {}

    /**
     * @brief returns N
     *
     * @return
     */
    int size() const {
        return this->m_matrix.size();
    }

    /**
     * @brief constructor from another metric
     *
     * @tparam OtherMetrics
     * @tparam ItemIterator
     * @param other
     * @param iBegin
     * @param iEnd
     */
    template <typename OtherMetrics, typename ItemIterator>
    array_metric(const OtherMetrics& other, ItemIterator iBegin, ItemIterator iEnd) :
        base(other, iBegin, iEnd, iBegin, iEnd) {}
};

}
}
#endif /* BASIC_METRICS_HPP */
