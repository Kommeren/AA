/**
 * @file graph_metrics.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */
#include <boost/graph/johnson_all_pairs_shortest.hpp>
#include "boost/multi_array.hpp"

namespace paal {
namespace data_structures {

namespace graph_type {
    class Sparse;
    class Dense;
    class Large;
}

template <typename Graph> struct GraphMetricTraits { 
    typedef graph_type::Sparse GraphTypeTag;
};


template <typename DistanceTypeParam> class MetricBase {
    public:
        typedef DistanceTypeParam DistanceType;
        MetricBase(int N) : m_matrix(boost::extents[N][N]) { }
        template <typename Vertex> DistanceType operator()(const Vertex & v, const Vertex & w) const {
            return m_matrix[v][w];
        }

    protected:
        static const int DIM_NR = 2;
        typedef boost::multi_array<DistanceType, DIM_NR> matrix_type; 
        matrix_type m_matrix;
};

//impplementation of different startegies of computing metric
namespace metric_fillers {

    //generic
    template <typename GraphTypeTag> class GraphMericFillerImpl {};
    
    template <> class GraphMericFillerImpl<graph_type::Sparse> {
        public:
        template <typename Graph, typename ResultMatrics> 
        void fillMatrix(const Graph & g, ResultMatrics & rm)  {
	    boost::johnson_all_pairs_shortest_paths(g, rm); 
        }
    };

    //TODO DENSE GRAPHS, LARGE GRAPHS
     
}


// GENERIC
// GraphTypeTag could be sparse, dense, large ...
template <typename Graph, typename DistanceType/*, typename VertexType*/, 
          typename GraphFiller = metric_fillers::GraphMericFillerImpl<typename GraphMetricTraits<Graph>::GraphTypeTag > > 
    class  GraphMetric : public MetricBase</*typename property_traits<DistanceTypePropertyMap>::value_type*/DistanceType>, public GraphFiller {
          typedef   MetricBase<DistanceType/*, typename boost::graph_traits<Graph>::vertex_descriptor*/> GMBase;

        public:
            GraphMetric(const Graph & g)  
                : GMBase(boost::num_vertices(g)) {
                fillMatrix(g, GMBase::m_matrix);
            }
};


//Specialization for adjacency_list
template <typename OutEdgeList, 
          typename VertexList, 
          typename Directed,
          typename VertexProperties, 
          typename EdgeProperties,
          typename GraphProperties, 
          typename EdgeList>
     struct GraphMetricTraits<boost::adjacency_list<OutEdgeList, 
                                  VertexList, 
                                  Directed,
                                  VertexProperties, 
                                  EdgeProperties,
                                  GraphProperties, 
                                  EdgeList>> {
                                    typedef graph_type::Sparse GraphTypeTag;
                                  };


} //data_structures
} //paal
