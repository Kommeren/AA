/**
 * @file dreyfus_wagner.hpp
 * @brief Finds optimal Steiner Tree in exponential time.
 * @author Maciej Andrejczuk
 * @version 1.0
 * @date 2013-07-05
 */
#ifndef DREYFUS_WAGNER_HPP
#define DREYFUS_WAGNER_HPP

#include <unordered_map>
#include <bitset>

#include "paal/data_structures/metric/metric_traits.hpp"
#include "paal/data_structures/metric/graph_metrics.hpp"

namespace paal {
namespace steiner_tree {

/**
 * Implements Dreyfus-Wagner algorithm.
 * The algorithm finds optimal Steiner Tree in exponential time, 3^k * n.
 */
template<typename Metric, typename Terminals, typename NonTerminals, unsigned int TerminalsLimit = 32>
class DreyfusWagner {
public:
    typedef data_structures::MetricTraits<Metric> MT;
    typedef typename MT::VertexType Vertex;
    typedef typename MT::DistanceType Dist;
    typedef typename std::pair<Vertex, Vertex> Edge;
    typedef typename std::bitset<TerminalsLimit> TerminalsBitSet;
    typedef std::pair<Vertex, TerminalsBitSet> State;

    /**
     * Constructor used for solving Steiner Tree problem.
     */
    DreyfusWagner(const Metric & costMap, const Terminals & term, const NonTerminals & nonTerminals) :
            m_costMap(costMap), m_terminals(term), m_nonTerminals(nonTerminals) {

        for (int i = 0; i < (int)m_terminals.size(); i++) {
            m_elementsMap[m_terminals[i]] = i;
        }
    }

    /**
     * Finds optimal Steiner Tree.
     * @param start Vertex to start the recurrence from.
     */
    void solve(int start = 0) {
        int n = m_elementsMap.size();
        assert(start >= 0 && start < n);
        TerminalsBitSet remaining;
        // set all terminals except 'start' to 1
        for (int i = 0; i < n; i++) {
            remaining.set(i);
        }
        remaining.reset(start);

        m_cost = connectVertex(m_terminals[start], remaining);
        retrieveSolutionConnect(m_terminals[start], remaining);

    }

    /**
     * Gets the optimal Steiner Tree cost.
     */
    Dist getCost() {
        return m_cost;
    }

    /**
     * Gets edges belonging to optimal tree.
     */
    const std::vector<Edge> & getEdges() {
        return m_edges;
    }

    /**
     * Gets selected Steiner vertices.
     */
    const std::set<Vertex> & getSteinerVertices() {
        return m_steinerElements;
    }

private:
    /*
     * @brief Computes minimal cost of connecting given vertex and a set of other vertices.
     * @param v vertex currently processed
     * @param mask vertices not yet processed has corresponding bits set to 1
     */
    Dist connectVertex(Vertex v, TerminalsBitSet remaining) {
        if (remaining.none()) {
            return 0;
        }
        if (remaining.count() == 1) {
            int k = smallestBit(remaining);
            Dist cost = m_costMap(v, m_terminals[k]);
            m_bestCand[codeState(v, remaining)] = std::make_pair(cost, m_terminals[k]);
            return cost;
        }
        // Check in the map if already computed
        if (m_bestCand.find(codeState(v, remaining)) != m_bestCand.end()) {
            return m_bestCand[codeState(v, remaining)].first;
        }
        Dist best = splitVertex(v, remaining);
        Vertex cand = v;

        for (Vertex w: m_nonTerminals) {
            Dist val = splitVertex(w, remaining);
            val += m_costMap(v, w);
            if (best < 0 || val < best) {
                best = val;
                cand = w;
            }
        }
        for (auto vertexAndTerminalId: m_elementsMap) {
            Vertex w = vertexAndTerminalId.first;
            int terminalId = vertexAndTerminalId.second;
            if (!remaining.test(terminalId)) continue;
            remaining.reset(terminalId);
            Dist val = connectVertex(w, remaining);
            val += m_costMap(v, w);
            remaining.set(terminalId);

            if (best < 0 || val < best) {
                best = val;
                cand = w;
            }
        }
        m_bestCand[codeState(v, remaining)] = std::make_pair(best, cand);
        return best;
    }

    /**
     * @brief Computes minimal cost by splitting the tree in two parts.
     */
    Dist splitVertex(Vertex v, TerminalsBitSet remaining) {
        if (remaining.count() < 2) {
            return 0;
        }
        // Check in the map if already computed
        if (m_bestSplit.find(codeState(v, remaining)) != m_bestSplit.end()) {
            return m_bestSplit[codeState(v, remaining)].first;
        }
        int k = smallestBit(remaining) + 1; // optimalization, to avoid checking subset twice
        std::pair<Dist, TerminalsBitSet> best = bestSplit(v, remaining, remaining, k);
        m_bestSplit[codeState(v, remaining)] = best;
        return best.first;
    }

    /**
     * Generates all splits of given set of vertices and finds the best one.
     */
    std::pair<Dist, TerminalsBitSet> bestSplit(const Vertex v, const TerminalsBitSet remaining, TerminalsBitSet subset, int k) {
        if (k == (int)m_terminals.size()) {
            TerminalsBitSet complement = remaining ^ subset;
            if (!subset.none() && !complement.none()) {
                Dist val = connectVertex(v, subset) + connectVertex(v, complement);
                return make_pair(val, subset);
            }
            else {
                return std::make_pair(-1, NULL);
            }
        } else {
            std::pair<Dist, TerminalsBitSet> ret1, ret2;
            ret1 = bestSplit(v, remaining, subset, k + 1);
            if (remaining.test(k)) {
                subset.flip(k);
                ret2 = bestSplit(v, remaining, subset, k + 1);
                if (ret1.first < 0 || ret1.first > ret2.first) {
                    ret1 = ret2;
                }
            }
            return ret1;
        }
    }

    /**
     * Retrieves the path of the optimal solution returned by connectVertex method.
     */
    void retrieveSolutionConnect(Vertex v, TerminalsBitSet remaining) {
        if (remaining.none())
            return;
        Vertex next = m_bestCand.at(codeState(v, remaining)).second;

        auto terminalIdIter = m_elementsMap.find(next);
        if (v == next) { // called wagner directly from dreyfus
            retrieveSolutionSplit(next, remaining);
        } else if (terminalIdIter == m_elementsMap.end()) { // nonterminal
            addVertex(next);
            addEdge(v, next);
            retrieveSolutionSplit(next, remaining);
        } else { // terminal
            addEdge(v, next);
            remaining.flip(terminalIdIter->second);
            retrieveSolutionConnect(next, remaining);
        }
    }

    /**
     * Retrieves the path of the optimal solution returned by splitVertex method.
     */
    void retrieveSolutionSplit(Vertex v, TerminalsBitSet remaining) {
        if (remaining.none())
            return;
        TerminalsBitSet split = m_bestSplit.at(codeState(v, remaining)).second;
        retrieveSolutionConnect(v, split);
        retrieveSolutionConnect(v, remaining ^ split);
    }

    /**
     * Codes current state to the structure that fits as the map key.
     */
    State codeState(Vertex v, TerminalsBitSet remaining) {
        // TODO: can be optimized
        return std::make_pair(v, remaining);
    }

    /**
     * Hash function for State, used in unordered_maps.
     */
    struct StateHash {
        std::size_t operator()(const State& k) const {
            return std::hash<Vertex>()(k.first) ^
                (std::hash<TerminalsBitSet>()(k.second) << 1);
        }
    };

    /**
     * Adds the edge to the solution.
     */
    void addEdge(Vertex u, Vertex w) {
        Edge e = std::make_pair(u, w);
        m_edges.push_back(e);
    }

    /**
     * Adds Steiner vertex to the solution.
     */
    void addVertex(Vertex v) {
        m_steinerElements.insert(v);
    }

    /**
     * Finds the index of the first nonempty bit in given mask.
     */
    int smallestBit(TerminalsBitSet mask) {
        int k = 0;
        while (!mask.test(k)) k++;
        return k;
    }

    const Metric& m_costMap; // stores the cost for each edge
    const Terminals& m_terminals; // terminals to be connected
    const NonTerminals& m_nonTerminals; // list of all non-terminals

    Dist m_cost; // cost of optimal Steiner Tree
    std::set<Vertex> m_steinerElements; // non-terminals selected for spanning tree
    std::vector<Edge> m_edges; // edges spanning the component

    std::unordered_map<Vertex, int> m_elementsMap; // maps Vertex to position in m_terminals vector
    typedef std::pair<Dist, Vertex> StateV;
    typedef std::pair<Dist, TerminalsBitSet> StateBM;
    std::unordered_map<State, StateV, StateHash> m_bestCand; // stores result of dreyfus method for given state
    std::unordered_map<State, StateBM, StateHash> m_bestSplit; // stores result of wagner method for given state
};

template <unsigned int TerminalsLimit = 32, typename Metric, typename Terminals, typename NonTerminals>
DreyfusWagner<Metric, Terminals, NonTerminals, TerminalsLimit>
make_DreyfusWagner(const Metric& metric, const Terminals& terminals, const NonTerminals& nonTerminals) {
    return DreyfusWagner<Metric, Terminals, NonTerminals, TerminalsLimit>(metric, terminals, nonTerminals);
}

} // steiner_tree
} //paal

#endif /* DREYFUS_WAGNER_HPP */
