/**
 * @file voronoi.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */
#include <set>
#include <map>
#include <cassert>

#ifndef __VORONOI__
#define __VORONOI__

#include <boost/range/adaptor/map.hpp>

namespace paal {
namespace data_structures {


template <typename Vertex, typename Metric>
class Voronoi {
    typedef std::multimap<Vertex, Vertex> GeneratorsToVertices;
    
    public:
        typedef Vertex VertexType;
        typedef std::set<Vertex> GeneratorsSet;
        typedef typename Metric::DistanceType Dist;
        //TODO change to vector
        typedef GeneratorsSet Vertices;
        typedef typename GeneratorsToVertices::const_iterator VerticesForGeneratorIter;
     //   typedef decltype( boost::make_transform_iterator(
       //           std::declval<VerticesForGeneratorPairIter>, helpers::choose_second<VertexType, VertexType>())) VerticesForGeneratorIter;

        Voronoi(const GeneratorsSet & generators, 
               Vertices vertices,
               const Metric & m) :
            m_vertices(std::move(vertices)), m_metric(m) {
                for(Vertex f : generators) {
                    addGenerator(f);
                }
            }

       
        // returns diff between new cost and old cost
        Dist addGenerator(Vertex f) {
            Dist cost = Dist();
            m_generators.insert(f);
           
            //first generatorsility
            if(m_generators.size() == 1) {
                m_verticesToGenerators.clear();
                m_generatorsToVertices.clear();
                for(Vertex v : m_vertices) {
                    m_verticesToGenerators[v] = 
                        m_generatorsToVertices.insert(std::make_pair(f, v));
                    cost += m_metric(v,f); 
                }
                
                //could be too tricky
                cost = -cost;
                
            } else {
                for(Vertex v: m_vertices) {
                    Dist d = m_metric(v,f) - dist(v);
                    if(d < 0) {
                        cost += d;
                        assign(v,f);
                    }
                }
            }
            return cost;
        }
        
        // returns diff between new cost and old cost
        Dist remGenerator(Vertex f) {
            Dist cost = Dist();
            if(m_generators.size() == 1) {
                //could be to tricky...
                cost = Dist();
                for(Vertex v : m_vertices) {
                    cost += dist(v);
                }
                m_verticesToGenerators.clear();
                m_generatorsToVertices.clear();
            } else {

                auto op = std::bind(std::not_equal_to<Vertex>(), f, std::placeholders::_1);
                auto begin = m_generatorsToVertices.lower_bound(f);
                auto end = m_generatorsToVertices.upper_bound(f);
                for(;begin != end; ) {
                    auto v = begin->second;
                    //using the generatorst that this is a map 
                    //(with other containers you have to be carefull cause of iter invalidation)
                    ++begin;
                    cost -= dist(v);
                    cost += adjustVertex(v, op);
                }
            }
            m_generators.erase(f);
            return cost;
        }

        const GeneratorsSet & getGenerators() const {
            return m_generators;
        }
        
        const Vertices & getVertices() const {
            return m_vertices;
        }

            decltype(std::pair<VerticesForGeneratorIter, VerticesForGeneratorIter>() | boost::adaptors::map_values) 
        getVerticesForGenerator(Vertex g) const {
           
            auto l = m_generatorsToVertices.lower_bound(g);
            auto u = m_generatorsToVertices.upper_bound(g);

            return std::make_pair(l, u) | boost::adaptors::map_values;
        }

    private:
        
        Dist dist(Vertex v) {
            return m_metric(v, vertexToGenerators(v));
        }
        
        Dist adjustVertex(Vertex v, std::function<bool(Vertex)> filter = [](Vertex v){return true;}) {
            bool init = true;
            Dist d;
            for(Vertex f : m_generators) {
                if(filter(f) &&  (init || m_metric(v,f) < d)) {
                    assign(v,f);
                    d = m_metric(v,f);
                    init = false;
                }
            }
            assert(!init);
            return d; 
        }

        Vertex vertexToGenerators(Vertex v) const {
            auto i = m_verticesToGenerators.find(v);
            assert(i != m_verticesToGenerators.end());
            return i->second->first;
        }

        void assign(Vertex v, Vertex f) {
            auto prev = m_verticesToGenerators[v];
            m_generatorsToVertices.erase(prev);
            m_verticesToGenerators[v] = 
                m_generatorsToVertices.insert(std::make_pair(f, v));
        }

        typedef std::map<Vertex, 
                typename GeneratorsToVertices::iterator> VerticesToGenerators;
        
        VerticesToGenerators m_verticesToGenerators;
        GeneratorsToVertices m_generatorsToVertices;
        Vertices m_vertices;
        GeneratorsSet m_generators;

        const Metric & m_metric;
};


};
};

#endif //__VORONOI__
