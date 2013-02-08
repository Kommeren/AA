#include <map>

#include <boost/range/join.hpp>

#include "helpers/iterator_helpers.hpp"
#include "data_structures/voronoi.hpp"

namespace paal {
namespace local_search {
namespace steiner_tree {


template <typename Metric, typename Voronoi> 
class SteinerTree {
public:
    typedef typename Metric::DistanceType Dist;
    typedef typename Metric::VertexType VertexType;
    typedef std::vector<VertexType> Dists;
    static const int SUSBSET_SIZE = 3;
    typedef typename kTuple<VertexType, SUSBSET_SIZE>::type ThreeTuple;
//    typedef std::vector<ThreeTuple> ThreeSubsetsToIndex;
//    typedef std::map<ThreeTuple, int> IndexToThreeSubsets;
    typedef std::map<ThreeTuple, Dist> ThreeSubsetsDists;
    typedef std::map<ThreeTuple, VertexType> NearstByThreeSubsets;
    
    SteinerTree(const Metric & m) : m_metric(m) {}


    void getSteinerTree(const Voronoi & voronoi) {
        typedef decltype(voronoi.getGenerators().begin()) TerminalIterator;
        typedef helpers::SubsetsIterator<TerminalIterator, SUSBSET_SIZE> ThreeSubsetsIter;
        //ThreeSubsetsToIndex subToIndex;
        ThreeSubsetsDists subsDists;
        NearstByThreeSubsets nearestVertex;

        auto terminalsBegin = voronoi.getGenerators().begin();
        auto terminalsEnd = voronoi.getGenerators().end();
        auto steinerBegin = voronoi.getVertices().begin();
        auto steinerEnd = voronoi.getVertices().end();

        ThreeSubsetsIter subBegin(terminalsBegin, terminalsEnd);
        ThreeSubsetsIter subEnd(terminalsEnd, terminalsEnd);
        
        //finding nearest vertex to subseet
        std::for_each(subBegin, subEnd, [&](const ThreeTuple & subset) {
            //TODO awfull coding, need to be changed to loop
            auto vRange1 =  voronoi.getVerticesForGenerator(std::get<0>(subset));
            auto vRange2 =  voronoi.getVerticesForGenerator(std::get<1>(subset));
            auto vRange3 =  voronoi.getVerticesForGenerator(std::get<2>(subset));
            auto range = boost::join(boost::join(vRange1, vRange2), vRange3);
            //int k = subToIndex.push_back(subsIndex);
            //indexToSub.insert(std::make_pair(subset, k));
            nearestVertex[subset] =  *std::min_element(boost::begin(range), boost::end(range), [&](VertexType v1, VertexType v2) {
                return this->dist(v1, subset) < this->dist(v2, subset);
            });
            subsDists[subset] = this->dist(nearestVertex[subset], subset);
        });
    }
private:

    //minor TODO could by more general somewhere
    Dist dist(VertexType v, const ThreeTuple & tup) {
        return   m_metric(v, std::get<0>(tup))
               + m_metric(v, std::get<1>(tup))
               + m_metric(v, std::get<2>(tup));
    }

    const Metric & m_metric;
};

} // steiner_tree
} //local_search
} //paal
