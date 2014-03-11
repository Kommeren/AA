/**
 * @file 2_local_search_components.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2014-03-11
 */
#ifndef TWO_LOCAL_SEARCH_COMPONENTS_HPP
#define TWO_LOCAL_SEARCH_COMPONENTS_HPP

#include "paal/data_structures/subset_iterator.hpp"

namespace paal {
namespace local_search {

    /**
     * @brief Swap
     *
     * @tparam Element
     */
template <typename Element>
struct Swap {
    /**
     * @brief constructor
     *
     * @param from
     * @param to
     */
    Swap(Element from, Element to) : m_from(from), m_to(to) {}

    /**
     * @brief getter for m_from
     *
     * @return
     */
    Element getFrom() const {
        return m_from;
    }

    /**
     * @brief getter for m_to
     *
     * @return
     */
    Element getTo() const {
        return m_to;
    }
private:
    Element m_from;
    Element m_to;
};

/**
 * @brief Functor creating Move
 */
struct MakeSwap {
    /**
     * @brief operator()
     *
     * @param from
     * @param to
     *
     * @return
     */
    template <typename Element>
    Swap<Element> operator()(Element from,
                    Element to) const {
        return Swap<Element>(from, to);
    }
};

/**
 * @brief gain for two opt moves
 *
 * @tparam Metric
 */
template <typename Metric>
    class GainTwoOpt {
    public:
        /**
         * @brief
         *
         * @param m fulfills \ref metric concept.
        */
        GainTwoOpt(const Metric & m) : m_metric(m) {}

        /**
         * @brief returns gain for given adjustment
         *
         * @tparam Solution
         * @tparam SolutionElement
         * @param swap
         *
         * @return
         */
        template <typename Solution, typename SolutionElement>
        int operator()(const Solution &, const Swap<SolutionElement> & swap) {
            auto from = swap.getFrom();
            auto to = swap.getTo();
            return m_metric(from.first, from.second) + m_metric(to.first, to.second) -
               m_metric(from.first, to.first) - m_metric(from.second, to.second);
        }

    private:
        const Metric & m_metric;
};

/**
 * @brief Commit class for local_search
 */
struct TwoLocalSearchCommit  {
    /**
     * @brief flips appropriate segment in the solution
     *
     * @tparam SolutionElement
     * @tparam Solution
     * @param s
     * @param swap
     */
    template <typename SolutionElement, typename Solution>
    bool operator()(Solution & s, const Swap<SolutionElement> & swap) {
        s.getCycle().flip(swap.getFrom().second, swap.getTo().first);
        return true;
    }
};

/**
 * @brief Commit class for local_search
 */
class TwoLocalSearchGetMoves  {

    /**
     * @brief Functor needed for type computation
     *
     * @tparam Solution
     */
    template <typename Solution>
    struct TypesEval {
        using SolutionIter = decltype(std::declval<Solution>().begin());
        using Subset = data_structures::SubsetsIterator<2, SolutionIter, MakeSwap>;
        using IterPair = std::pair<Subset, Subset>;
    };

public:
    /**
     * @brief return all pairs of elements from solution
     *
     * @tparam Solution
     * @param solution
     */
    template <typename Solution>
    auto operator()(Solution & solution) const ->
        typename TypesEval<Solution>::IterPair
    {
        return data_structures::make_SubsetsIteratorRange<2>
                        (solution.begin(), solution.end(), MakeSwap{});
    }
};

}//!local_search
}//!paal

#endif /* TWO_LOCAL_SEARCH_COMPONENTS_HPP */
