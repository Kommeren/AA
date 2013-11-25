/**
 * @file basic_metrics.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-15
 */
#ifndef BASIC_METRICS_HPP
#define BASIC_METRICS_HPP 

#include <boost/multi_array.hpp>
#include <array>
#include "metric_traits.hpp"

namespace paal {
namespace data_structures {

/**
 * @class AssymetricArrayMetric
 * @brief \ref metric implementation on 2 dimensional array
 *
 * @tparam DistanceTypeParam
 */
template <typename DistanceTypeParam> 
class AssymetricArrayMetric {
    public:
        typedef DistanceTypeParam DistanceType;
        typedef int VertexType;
        AssymetricArrayMetric(int N = 0, int M = 0) : m_matrix(boost::extents[N][M]) { }
        
        DistanceType operator()(const VertexType & v, const VertexType & w) const {
            return m_matrix[v][w];
        }
        
        DistanceType & operator()(const VertexType & v, const VertexType & w) {
            return m_matrix[v][w];
        }

        int size() const {
            return m_matrix.size();
        }
    
        template <typename OtherMetrics, typename XIterator, typename YIterator>
        AssymetricArrayMetric(const OtherMetrics& other, 
                   XIterator xBegin, XIterator xEnd, 
                   YIterator yBegin, YIterator yEnd) : 
                        AssymetricArrayMetric(std::distance(xBegin, xEnd),
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

        AssymetricArrayMetric & operator=(const AssymetricArrayMetric & am) {
            auto shape = am.m_matrix.shape();
            std::vector<std::size_t> dim(shape, shape + DIM_NR);
            m_matrix.resize(dim);
            m_matrix = am.m_matrix;
            return *this; 
        }

    protected:
        static const int DIM_NR = 2;
        typedef boost::multi_array<DistanceType, DIM_NR> matrix_type; 
        matrix_type m_matrix;
};

template <typename DistanceTypeParam> 
class ArrayMetric : public AssymetricArrayMetric<DistanceTypeParam> {
    typedef AssymetricArrayMetric<DistanceTypeParam> base;
public:
    ArrayMetric(int N = 0) : base(N, N) {}

    template <typename OtherMetrics, typename ItemIterator>
    ArrayMetric(const OtherMetrics& other, ItemIterator iBegin, ItemIterator iEnd) :
        base(other, iBegin, iEnd, iBegin, iEnd) {}
};

}
}
#endif /* BASIC_METRICS_HPP */
