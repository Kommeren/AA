/**
 * @file steiner_strategy.hpp
 * @brief
 * @author Maciej Andrejczuk
 * @version 1.0
 * @date 2013-08-01
 */
#ifndef STEINER_STRATEGY_HPP
#define STEINER_STRATEGY_HPP

#include "paal/iterative_rounding/steiner_tree/steiner_components.hpp"
#include "paal/utils/random.hpp"

namespace paal {
namespace ir {

/**
 * Generates all the components possible.
 * It iterates over all subsets of terminals with no more than K elements.
 */
class AllGenerator {
public:
    AllGenerator(int K = 4) : m_componentMaxSize(K) {}

    template<typename Metric, typename Terminals>
    void genComponents(const Metric& costMap, const Terminals& terminals,
            const Terminals& steinerVertices,
            SteinerComponents<
                typename data_structures::MetricTraits<Metric>::VertexType,
                typename data_structures::MetricTraits<Metric>::DistanceType>& components) {

        typedef typename data_structures::MetricTraits<Metric>::VertexType Vertex;
        typedef typename data_structures::MetricTraits<Metric>::DistanceType Dist;
        std::vector<Vertex> tmp;
        genAllComponents<Vertex, Dist>(components, 0, terminals.size(), tmp,
                costMap, terminals, steinerVertices);
    }
private:
    template<typename Vertex, typename Dist, typename Metric, typename Terminals>
    void genAllComponents(SteinerComponents<Vertex, Dist>& components,
            int firstAvail, int last, std::vector<Vertex>& curr,
            const Metric& costMap, const Terminals& terminals,
            const Terminals& steinerVertices) {

        if (curr.size() > 1) {
            SteinerComponent<Vertex, Dist> c(curr, costMap, terminals, steinerVertices);
            components.add(std::move(c));
        }
        if ((int) curr.size() >= m_componentMaxSize)
            return;
        for (int i = firstAvail; i < last; ++i) {
            curr.push_back(terminals[i]);
            genAllComponents(components, i + 1, last, curr, costMap, terminals, steinerVertices);
            curr.pop_back();
        }
    }

    int m_componentMaxSize;
};

/**
 * Generates specified number of components by selecting random elements.
 */
class RandomGenerator {
public:
    RandomGenerator(int N = 100, int K = 3) :
            m_iterations(N), m_componentMaxSize(K) {
    }

    template<typename Metric, typename Terminals>
    void genComponents(const Metric& costMap, const Terminals & terminals,
            const Terminals& steinerVertices,
            SteinerComponents<
                typename data_structures::MetricTraits<Metric>::VertexType,
                typename data_structures::MetricTraits<Metric>::DistanceType>& components) {

        typedef typename data_structures::MetricTraits<Metric>::VertexType Vertex;
        typedef typename data_structures::MetricTraits<Metric>::DistanceType Dist;
        if (terminals.size() < 2) {
            return;
        }
        for (int i = 0; i < m_iterations; ++i) {
            std::set<Vertex> curr;
            while ((int) curr.size() < m_componentMaxSize) {
                if (curr.size() > 1) {
                    int c = (int) rand() % m_componentMaxSize; // TODO: Is this fair probability?
                    if (c == 0) {
                        break;
                    }
                }
                int r = (int) rand() % terminals.size();
                curr.insert(terminals[r]);
            }
            std::vector<Vertex> elements(curr.begin(), curr.end());
            SteinerComponent<Vertex, Dist> c(elements, costMap, terminals, steinerVertices);
            components.add(std::move(c));
        }
    }
private:
    int m_iterations;
    int m_componentMaxSize;
};

/**
 * Generates specified number of components by randomly selecting elements
 * with probability dependent on distance from vertices already selected.
 */
class SmartGenerator {
public:
    SmartGenerator(int N = 100, int K = 3) :
            m_iterations(N), m_componentMaxSize(K) {
    }

    template<typename Metric, typename Terminals>
    void genComponents(const Metric& costMap, const Terminals& terminals,
            const Terminals& steinerVertices,
            SteinerComponents<
                    typename data_structures::MetricTraits<Metric>::VertexType,
                    typename data_structures::MetricTraits<Metric>::DistanceType>& components) {

        typedef typename data_structures::MetricTraits<Metric>::VertexType Vertex;
        typedef typename data_structures::MetricTraits<Metric>::DistanceType Dist;
        std::vector<Vertex> elements;
        std::vector<double> prob;
        for (Vertex start : terminals) {
            for (int i = 0; i < m_iterations; ++i) {
                elements.clear();
                elements.push_back(start);
                int limit = 2 + rand() % (m_componentMaxSize - 1);
                while ((int) elements.size() < limit) {
                    prob.resize(terminals.size());
                    for (int k = 0; k < (int) prob.size(); ++k) {
                        for (auto e : elements) {
                            if (e == terminals[k]) {
                                prob[k] = 0;
                                break;
                            }
                            int cost = costMap(e, elements[k]);
                            assert(cost > 0);
                            prob[k] = std::max(prob[k], 1.0 / cost);
                        }
                    }
                    int selected = paal::utils::randomSelect<false>(prob.begin(), prob.end()) - prob.begin();
                    if (selected < 0)
                        break;
                    elements.push_back(terminals[selected]);
                }
                SteinerComponent<Vertex, Dist> c(elements, costMap, terminals, steinerVertices);
                components.add(std::move(c));
            }
        }
    }
private:
    int m_iterations;
    int m_componentMaxSize;
};

} //ir
} //paal

#endif /* STEINER_STRATEGY_HPP */
