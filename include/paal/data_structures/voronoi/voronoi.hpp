/**
 * @file voronoi.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */
#ifndef VORONOI_HPP
#define VORONOI_HPP


#include <unordered_set>
#include <unordered_map>
#include <map>
#include <cassert>
#include <climits>


#include <boost/range/adaptor/map.hpp>
#include <boost/functional/hash.hpp>

#include "paal/data_structures/metric/metric_traits.hpp"
#include "paal/utils/functors.hpp"
#include "voronoi_traits.hpp"

namespace paal {
namespace data_structures {


/**
 * @class Voronoi
 * @brief simple implementation of the \ref voronoi concept.
 *
 * @tparam Metric
 */
template <typename Metric>
class Voronoi {
    public:
        typedef typename MetricTraits<Metric>::VertexType VertexType;
        typedef std::multimap<VertexType, VertexType> GeneratorsToVertices;
        typedef std::unordered_set<VertexType, boost::hash<VertexType>> GeneratorsSet;
        typedef typename MetricTraits<Metric>::DistanceType Dist;
        //TODO change to vector
        typedef GeneratorsSet Vertices;
        typedef typename GeneratorsToVertices::const_iterator VerticesForGeneratorIter;

        /**
         * @brief Constructor
         *
         * @param generators
         * @param vertices
         * @param m
         * @param costOfNoGenerator
         */
        Voronoi(const GeneratorsSet & generators,
               Vertices vertices,
               const Metric & m, Dist costOfNoGenerator = std::numeric_limits <Dist>::max()) :
            m_vertices(std::move(vertices)), m_metric(m), m_costOfNoGenerator(costOfNoGenerator) {
                for(VertexType f : generators) {
                    addGenerator(f);
                }
            }

        /**
         * @brief Copy constructor
         *
         * @param v
         */
        Voronoi(const Voronoi & v) : m_generatorsToVertices(v.m_generatorsToVertices),
                m_vertices(v.m_vertices), m_generators(v.m_generators),
                m_metric(v.m_metric), m_costOfNoGenerator(v.m_costOfNoGenerator) {
            auto b = m_generatorsToVertices.begin();
            auto e = m_generatorsToVertices.end();
            for(;b!=e; ++b) {
                m_verticesToGenerators.insert(std::make_pair(b->second, b));
            }
        }

        /**
         * @brief Move constructor
         *
         * @param v
         */
        Voronoi(Voronoi && v) : m_verticesToGenerators(std::move(v.m_verticesToGenerators)),
                                m_generatorsToVertices(std::move(v.m_generatorsToVertices)),
                m_vertices(std::move(v.m_vertices)), m_generators(std::move(v.m_generators)),
                m_metric(v.m_metric), m_costOfNoGenerator(v.m_costOfNoGenerator) {
        }

        /// returns diff between new cost and old cost
        Dist addGenerator(VertexType f) {
            Dist cost = Dist();
            m_generators.insert(f);

            //first generatorsility
            if(m_generators.size() == 1) {
                m_verticesToGenerators.clear();
                m_generatorsToVertices.clear();
                for(VertexType v : m_vertices) {
                    m_verticesToGenerators[v] =
                        m_generatorsToVertices.insert(std::make_pair(f, v));
                    cost += m_metric(v,f);
                }

                cost = cost - m_costOfNoGenerator;

            } else {
                for(VertexType v: m_vertices) {
                    Dist d = m_metric(v,f) - dist(v);
                    if(d < 0) {
                        cost += d;
                        assign(v,f);
                    }
                }
            }
            return cost;
        }

        /// returns diff between new cost and old cost
        Dist remGenerator(VertexType f) {
            Dist cost = Dist();
            if(m_generators.size() == 1) {
                cost = m_costOfNoGenerator;
                for(VertexType v : m_vertices) {
                    cost -= dist(v);
                }
                m_verticesToGenerators.clear();
                m_generatorsToVertices.clear();
            } else {
                auto op = std::bind(utils::NotEqualTo(), f, std::placeholders::_1);
                auto begin = m_generatorsToVertices.lower_bound(f);
                auto end = m_generatorsToVertices.upper_bound(f);
                for(;begin != end; ) {
                    auto v = begin->second;
                    //using the fact that generators is a map
                    //(with other containers you have to be careful cause of iter invalidation)
                    ++begin;
                    cost -= dist(v);
                    cost += adjustVertex(v, op);
                }
            }
            m_generators.erase(f);
            return cost;
        }

        /**
         * @brief getter for generators
         *
         * @return
         */
        const GeneratorsSet & getGenerators() const {
            return m_generators;
        }

        /**
         * @brief getter for vertices
         *
         * @return
         */
        const Vertices & getVertices() const {
            return m_vertices;
        }

        /**
         * @brief getter for vertices assigned to specific generator
         *
         * @param g
         */
            decltype(std::pair<VerticesForGeneratorIter, VerticesForGeneratorIter>() | boost::adaptors::map_values)
        getVerticesForGenerator(VertexType g) const {
            return m_generatorsToVertices.equal_range(g) | boost::adaptors::map_values;
        }

    private:

            /**
             * @brief distance of vertex to closest generator
             *
             * @param v
             *
             * @return
             */
        Dist dist(VertexType v) {
            return m_metric(v, vertexToGenerators(v));
        }

        /**
         * @brief find new generator for vertex
         *             only generators satisfying filer condition are considered
         *
         * @tparam Filter
         * @param v
         * @param filter
         *
         * @return
         */
        template <typename Filter = utils::ReturnTrueFunctor>
        Dist adjustVertex(VertexType v, Filter filter = Filter()) {
            bool init = true;
            Dist d = Dist();
            VertexType f_best = VertexType();
            for(VertexType f : m_generators) {
                if(filter(f)) {
                    Dist td = m_metric(v,f);
                    if(init || td < d) {
                        f_best = f;
                        d = td;
                        init = false;
                    }
                }
            }
            assert(!init);
            assign(v,f_best);
            return d;
        }

        /**
         * @brief get generator for given vertex
         *
         * @param v
         *
         * @return
         */
        VertexType vertexToGenerators(VertexType v) const {
            auto i = m_verticesToGenerators.find(v);
            assert(i != m_verticesToGenerators.end());
            return i->second->first;
        }


        /**
         * @brief assign vertex to generator
         *
         * @param v
         * @param f
         */
        void assign(VertexType v, VertexType f) {
            auto prev = m_verticesToGenerators.at(v);
            m_generatorsToVertices.erase(prev);
            m_verticesToGenerators[v] =
                m_generatorsToVertices.insert(std::make_pair(f, v));
        }

        typedef std::unordered_map<VertexType,
                typename GeneratorsToVertices::iterator, boost::hash<VertexType>> VerticesToGenerators;

        VerticesToGenerators m_verticesToGenerators;
        GeneratorsToVertices m_generatorsToVertices;
        Vertices m_vertices;
        GeneratorsSet m_generators;

        const Metric & m_metric;
        const Dist m_costOfNoGenerator;
};

/**
 * @brief specialization of VoronoiTraits
 *
 * @tparam Metric
 */
template <typename Metric>
struct VoronoiTraits<Voronoi<Metric>> :
    public  _VoronoiTraits<Voronoi<Metric>, typename MetricTraits<Metric>::VertexType> {
};

};
};

#endif //VORONOI_HPP
