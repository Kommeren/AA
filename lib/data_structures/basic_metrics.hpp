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

namespace paal {
namespace data_structures {

template <typename DistanceTypeParam> class ArrayMetric {
    public:
        typedef DistanceTypeParam DistanceType;
        typedef int VertexType;
        ArrayMetric(int N) : m_matrix(boost::extents[N][N]) { }
        template <typename Vertex> DistanceType operator()(const Vertex & v, const Vertex & w) const {
            return m_matrix[v][w];
        }
        
        template <typename Vertex> ArrayMetric<DistanceType> & set(const Vertex & v, const Vertex & w, DistanceType d)  {
            m_matrix[v][w] = d;
            return *this;
        }

        int size() const {
            return m_matrix.size();
        }


    protected:
        static const int DIM_NR = 2;
        typedef boost::multi_array<DistanceType, DIM_NR> matrix_type; 
        matrix_type m_matrix;
};

}
}
#endif /* BASIC_METRICS_HPP */
