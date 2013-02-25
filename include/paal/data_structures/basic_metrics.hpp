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

namespace paal {
namespace data_structures {

template <typename DistanceTypeParam> class ArrayMetric {
    public:
        typedef DistanceTypeParam DistanceType;
        typedef int VertexType;
        ArrayMetric(int N = 0) : m_matrix(boost::extents[N][N]) { }
        
        template <typename Vertex> DistanceType operator()(const Vertex & v, const Vertex & w) const {
            return m_matrix[v][w];
        }
        
        template <typename Vertex> DistanceType & operator()(const Vertex & v, const Vertex & w) {
            return m_matrix[v][w];
        }

        int size() const {
            return m_matrix.size();
        }

        ArrayMetric & operator=(const ArrayMetric & am) {
            auto shape = am.m_matrix.shape();
            std::vector<size_t> dim(shape, shape + DIM_NR);
            m_matrix.resize(dim);
            m_matrix = am.m_matrix;
            return *this; 
        }


    protected:
        static const int DIM_NR = 2;
        typedef boost::multi_array<DistanceType, DIM_NR> matrix_type; 
        matrix_type m_matrix;
};

}
}
#endif /* BASIC_METRICS_HPP */
