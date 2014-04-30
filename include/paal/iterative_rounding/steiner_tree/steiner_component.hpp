/**
 * @file steiner_component.hpp
 * @brief
 * @author Maciej Andrejczuk
 * @version 1.0
 * @date 2013-08-01
 */
#ifndef STEINER_COMPONENT_HPP
#define STEINER_COMPONENT_HPP


#include "paal/steiner_tree/dreyfus_wagner.hpp"
#include "paal/data_structures/metric/metric_traits.hpp"
#include "paal/data_structures/metric/graph_metrics.hpp"


#include <set>
#include <iosfwd>

namespace paal {
namespace ir {

/**
 * @class steiner_component
 * @brief Class represents k-components of Steiner Tree.
 * Component is a subtree whose terminals coincide with leaves.
 */
template <typename Vertex, typename Dist>
class steiner_component {
public:
    typedef typename std::pair<Vertex, Vertex> Edge;

    template<typename Metric, typename Terminals>
    steiner_component(std::vector<Vertex> & el, const Metric & costMap,
            const Terminals & term, const Terminals& steinerVertices) :
        m_elements(el), m_size(el.size()) {
        auto dw = paal::steiner_tree::make_dreyfus_wagner(costMap, m_elements, steinerVertices);
        dw.solve();
        m_cost = dw.get_cost();
        auto & steiner = dw.steiner_tree_zelikovsky11per6approximation();
        m_steiner_elements.insert(m_steiner_elements.begin(), steiner.begin(), steiner.end());
        m_edges = std::move(dw.get_edges());
    }

    /**
     * @brief Each component has versions, where sink is chosen from its terminals
     */
    Vertex get_sink(int version) const {
        assert(version < count_terminals());
        return m_elements[version];
    }

    /**
     * Returns vector composed of component's terminals.
     */
    const std::vector<Vertex>& get_elements() const {
        return m_elements;
    }

    /**
     * Returns vector composed of component's nonterminals, i.e. Steiner elements.
     */
    const std::vector<Vertex>& get_steiner_elements() const {
        return m_steiner_elements;
    }

    /**
     * Returns edges spanning the component.
     */
    const std::vector<Edge>& get_edges() const {
        return m_edges;
    }

    /**
     * Returns degree of component, i.e. number of terminals.
     */
    int count_terminals() const {
        return m_size;
    }

    /**
     * Returns minimal cost of spanning a component.
     */
    Dist get_cost() const {
        return m_cost;
    }

    /**
     * Prints the component.
     */
    friend std::ostream& operator<< (std::ostream& stream, const steiner_component& component) {
        for (int i = 0; i < component.m_size; i++) {
            stream << component.m_elements[i] << " ";
        }
        stream << ": ";
        for (auto edge: component.m_edges) {
            stream << "(" << edge.first << "," << edge.second << ") ";
        }
        stream << component.m_cost;
        return stream;
    }

private:
    const std::vector<Vertex> m_elements; // terminals of the component
    int m_size; // m_elements.size()
    Dist m_cost; // minimal cost of spanning the component
    std::vector<Vertex> m_steiner_elements; // non-terminals selected for spanning tree
    std::vector<Edge> m_edges; // edges spanning the component
};

} //ir
} //paal

#endif /* STEINER_COMPONENT_HPP */
