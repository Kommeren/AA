/**
 * @file steiner_tree.hpp
 * @brief
 * @author Maciej Andrejczuk
 * @version 1.0
 * @date 2013-08-01
 */
#ifndef STEINER_TREE_HPP
#define STEINER_TREE_HPP

#include <boost/range/join.hpp>

#include "paal/iterative_rounding/iterative_rounding.hpp"
#include "paal/iterative_rounding/ir_components.hpp"
#include "paal/lp/lp_row_generation.hpp"
#include "paal/utils/random.hpp"
#include "paal/data_structures/metric/basic_metrics.hpp"
#include "paal/iterative_rounding/steiner_tree/steiner_tree_oracle.hpp"
#include "paal/iterative_rounding/steiner_tree/steiner_components.hpp"
#include "paal/iterative_rounding/steiner_tree/steiner_strategy.hpp"
#include "paal/iterative_rounding/steiner_tree/steiner_utils.hpp"

namespace paal {
namespace ir {

namespace {
struct SteinerTreeCompareTraits {
    static const double EPSILON;
};

const double SteinerTreeCompareTraits::EPSILON = 1e-10;
}

/**
 * @class SteinerTree
 */
template<typename OrigMetric, typename Terminals, typename Result,
    typename Strategy=AllGenerator>
class SteinerTree {
public:
    typedef data_structures::MetricTraits<OrigMetric> MT;
    typedef typename MT::VertexType Vertex;
    typedef typename MT::DistanceType Dist;
    typedef typename std::pair<Vertex, Vertex> Edge;
    typedef utils::Compare<double> Compare;
    typedef data_structures::ArrayMetric<Dist> Metric;

    SteinerTree(const OrigMetric& metric, const Terminals& terminals,
            const Terminals& steinerVertices, Result result,
            const Strategy& strategy = Strategy()) :
        m_costMap(metric, boost::begin(boost::range::join(terminals, steinerVertices)),
                boost::end(boost::range::join(terminals, steinerVertices))),
        m_terminals(terminals), m_steinerVertices(steinerVertices),
        m_strategy(strategy), m_resultIterator(result),
        m_compare(SteinerTreeCompareTraits::EPSILON) {
    }

    /**
     * Move constructor
     */
    SteinerTree(SteinerTree&& other) = default;

    /**
     * Generates all the components using specified strategy.
     */
    void genComponents() {
        m_strategy.genComponents(m_costMap, m_terminals, m_steinerVertices, m_components);
        //std::cout << "Generated: " << m_components.size() << " components\n";
    }

    /**
     * Gets reference to all the components.
     */
    const SteinerComponents<Vertex, Dist>& getComponents() const {
        return m_components;
    }

    /**
     * Gets reference to all the terminals.
     */
    const Terminals& getTerminals() const {
        return m_terminals;
    }

    /**
     * Adds map entry from component id to LP ColId.
     */
    void addColumnLP(int id, ColId col) {
        bool b = m_elementsMap.insert(std::make_pair(id, col)).second;
        assert(b);
    }

    /**
     * Finds LP ColId based on component id.
     */
    ColId findColumnLP(int id) const {
        return m_elementsMap.at(id);
    }

    void addToSolution(const std::vector<Vertex>& steinerElements) {
        std::copy(steinerElements.begin(), steinerElements.end(), m_resultIterator);
    }

    /**
     * Recalculates distances after two vertices were merged.
     */
    void mergeVertices(Vertex u, Vertex w) {
        auto allElements = boost::range::join(m_terminals, m_steinerVertices);
        for (Vertex i: allElements) {
            for (Vertex j: allElements) {
                Dist x = m_costMap(i, u) + m_costMap(w, j);
                m_costMap(i, j) = std::min(m_costMap(i, j), x);
            }
        }
    }

    /**
     * Merges a component into its sink.
     */
    void updateGraph(const SteinerComponent<Vertex, Dist>& selected) {
        const std::vector<Vertex>& v = selected.getElements();
        auto allElementsExceptFirst = boost::make_iterator_range(++v.begin(), v.end());
        for (auto e : allElementsExceptFirst) {
            mergeVertices(v[0], e);
            auto ii = std::find(m_terminals.begin(), m_terminals.end(), e);
            assert(ii != m_terminals.end());
            m_terminals.erase(ii);
        }
        // Clean components, they will be generated once again
        m_components.clear();
        m_elementsMap.clear();
    }

    /**
     * Gets comparison method.
     */
    utils::Compare<double> getCompare() const {
        return m_compare;
    }

private:
    Metric m_costMap; // metric in current state
    Terminals m_terminals; // terminals in current state
    Terminals m_steinerVertices; // vertices that are not terminals
    SteinerComponents<Vertex, Dist> m_components; // components in current state
    Strategy m_strategy; // strategy to generate the components
    Result m_resultIterator; // list of selected Steiner Vertices
    Compare m_compare; // comparison method

    std::unordered_map<int, ColId> m_elementsMap; // maps componentId -> ColId in LP
};


class SteinerTreeInit {
public:
    /**
     * Initializes LP.
     */
    template <typename Problem, typename LP>
    void operator()(Problem& problem, LP & lp) {
        lp.clear();
        lp.setLPName("steiner tree");
        problem.genComponents();
        lp.setMinObjFun();
        addVariables(problem, lp);
        lp.loadMatrix();
    }
private:
    /**
     * Adds all the components as columns of LP.
     */
    template <typename Problem, typename LP>
    void addVariables(Problem& problem, LP & lp) {
        for (int i = 0; i < problem.getComponents().size(); ++i) {
            ColId col = lp.addColumn(problem.getComponents().find(i).getCost(), DB, 0, 1);
            problem.addColumnLP(i, col);
        }
    }
};

/**
 * Round Condition: step of iterative-randomized rounding algorithm.
 */
class SteinerTreeRoundCondition {
public:
    SteinerTreeRoundCondition() {}

    /**
     * Selects one component according to probability, adds it to solution and merges selected vertices.
     */
    template<typename Problem, typename LP>
    void operator()(Problem& problem, LP& lp) {
        std::vector<double> weights;
        weights.reserve(problem.getComponents().size());
        for (int i = 0; i < problem.getComponents().size(); ++i) {
            ColId cId = problem.findColumnLP(i);
            weights.push_back(lp.getColPrim(cId));
        }
        int selected = paal::utils::randomSelect(weights.begin(), weights.end()) - weights.begin();
        const auto & comp = problem.getComponents().find(selected);
        problem.addToSolution(comp.getSteinerElements());
        problem.updateGraph(comp);
        SteinerTreeInit()(problem, lp);
    }
};

class SteinerTreeStopCondition {
public:
    template<typename Problem>
    bool operator()(Problem& problem) {
        return problem.getTerminals().size() < 2;
    }
};

/**
 * Makes SteinerTree object. Just to avoid providing type names in template.
 */
template<typename OrigMetric, typename Terminals, typename Result,
        typename Strategy>
SteinerTree<OrigMetric, Terminals, Result, Strategy> make_SteinerTree(
        const OrigMetric& metric, const Terminals& terminals,
        const Terminals& steinerVertices, Result result, const Strategy& strategy) {
    return SteinerTree<OrigMetric, Terminals, Result, Strategy>(metric,
            terminals, steinerVertices, result, strategy);
}

template <
         typename Vertex,
         typename Dist,
         typename Components,
         typename SolveLPToExtremePoint = RowGenerationSolveLP<SteinerTreeOracle<Vertex, Dist, Components> >,
         typename RoundCondition = SteinerTreeRoundCondition,
         typename RelaxCondition = utils::ReturnFalseFunctor,
         typename StopCondition = SteinerTreeStopCondition,
         typename Init = SteinerTreeInit,
         typename SetSolution = utils::SkipFunctor>
             using  SteinerTreeIRComponents = IRComponents<SolveLPToExtremePoint, RoundCondition,
                         RelaxCondition, Init, SetSolution, StopCondition>;


template <typename OrigMetric, typename Terminals, typename Result,
    typename Strategy, typename IRComponents, typename Visitor = TrivialVisitor>
void steiner_tree_iterative_rounding(const OrigMetric& metric, const Terminals& terminals, const Terminals& steinerVertices,
        Result result, Strategy strategy, IRComponents comps, Visitor vis = Visitor()) {

    auto steiner = paal::ir::make_SteinerTree(metric, terminals, steinerVertices, result, strategy);
    paal::ir::solve_dependent_iterative_rounding(steiner, std::move(comps), std::move(vis));
}

template <typename OrigMetric, typename Terminals, typename Result,
    typename Strategy = AllGenerator>
void solve_steiner_tree(const OrigMetric& metric, const Terminals& terminals, const Terminals& steinerVertices,
        Result result, Strategy strategy=Strategy()) {
    typedef data_structures::MetricTraits<OrigMetric> MT;
    typedef typename MT::VertexType Vertex;
    typedef typename MT::DistanceType Dist;
    paal::ir::SteinerTreeOracle<Vertex, Dist, Terminals> oracle;
    paal::ir::SteinerTreeIRComponents<Vertex, Dist, Terminals> comps(make_RowGenerationSolveLP(oracle));
    steiner_tree_iterative_rounding(metric, terminals, steinerVertices, result, strategy, comps);
}

} //ir
} //paal
#endif /* STEINER_TREE_HPP */
