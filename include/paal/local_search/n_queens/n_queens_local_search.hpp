/**
 * @file n_queens_local_search.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2014-01-02
 */
#ifndef N_QUEENS_LOCAL_SEARCH_HPP
#define N_QUEENS_LOCAL_SEARCH_HPP

#define BOOST_RESULT_OF_USE_DECLTYPE
#include "paal/local_search/local_search.hpp"
#include "paal/local_search/search_components.hpp"

#include "paal/local_search/n_queens/n_queens_solution.hpp"
#include "paal/local_search/n_queens/n_queens_components.hpp"

namespace paal {
namespace local_search {

    /**
     * @brief NQueen Compoenents
     *
     * @tparam Args
     */
    template <typename... Args>
    using n_queens_local_search_components =  data_structures::components<
            data_structures::NameWithDefault<get_moves, n_queensget_moves>,
            data_structures::NameWithDefault<Gain, n_queens_gain>,
            data_structures::NameWithDefault<Commit, n_queens_commit>
                >::type<Args...>;

    /**
     * @brief n queen local search
     *
     * @tparam SearchStrategy
     * @tparam PostSearchAction
     * @tparam GlobalStopCondition
     * @tparam NQueensPositionsVector
     * @tparam components
     * @param pos
     * @param psa
     * @param gsc
     * @param nQueenscomponents
     *
     * @return
     */
    template <typename SearchStrategy,
             typename PostSearchAction,
             typename GlobalStopCondition,
             typename NQueensPositionsVector,
             typename... components>
                 bool n_queens_solution_local_search(
                         NQueensPositionsVector & pos,
                         SearchStrategy searchStrategy,
                         PostSearchAction psa,
                         GlobalStopCondition gsc,
                         components... nQueenscomponents) {
                     n_queens_solution_adapter<NQueensPositionsVector> nqueens(pos);
                     return local_search(nqueens, std::move(searchStrategy), std::move(psa),
                                std::move(gsc), std::move(nQueenscomponents)...);
                 }

    /**
     * @brief n queen local search (simple version)
     *
     * @tparam NQueensPositionsVector
     * @tparam components
     * @param pos
     * @param nQueenscomponents
     */
    template <typename NQueensPositionsVector, typename... components>
        void n_queens_solution_local_search_simple(NQueensPositionsVector & pos, components... nQueenscomponents) {
            n_queens_solution_local_search(pos,
                                       choose_first_better_strategy{},
                                       utils::skip_functor{},
                                       utils::return_false_functor{},
                                       std::move(nQueenscomponents)...);
        }

} //!local_search
} // !paal

#endif /* N_QUEENS_LOCAL_SEARCH_HPP */
