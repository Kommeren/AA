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
#include "paal/local_search/single_solution/local_search_single_solution.hpp"
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
    using NQueensLocalSearchComponents =  data_structures::Components<
            data_structures::NameWithDefault<GetMoves, NQueensGetMoves>,
            data_structures::NameWithDefault<Gain, NQueensGain>,
            data_structures::NameWithDefault<Commit, NQueensCommit>
                >::type<Args...>;

    /**
     * @brief n queen local search
     *
     * @tparam SearchStrategy
     * @tparam PostSearchAction
     * @tparam GlobalStopCondition
     * @tparam NQueensPositionsVector
     * @tparam Components
     * @param pos
     * @param psa
     * @param gsc
     * @param nQueensComponents
     *
     * @return
     */
    template <typename SearchStrategy = search_strategies::ChooseFirstBetter,
             typename PostSearchAction,
             typename GlobalStopCondition,
             typename NQueensPositionsVector,
             typename... Components>
                 bool nQueensSolutionLocalSearch(
                         NQueensPositionsVector & pos,
                         PostSearchAction psa,
                         GlobalStopCondition gsc,
                         Components... nQueensComponents) {
                     NQueensSolutionAdapter<NQueensPositionsVector> nqueens(pos);
                     return local_search(nqueens, std::move(psa),
                                std::move(gsc), std::move(nQueensComponents)...);
                 }

    /**
     * @brief n queen local search (simple version)
     *
     * @tparam NQueensPositionsVector
     * @tparam Components
     * @param pos
     * @param nQueensComponents
     */
    template <typename NQueensPositionsVector, typename... Components>
        void nQueensSolutionLocalSearchSimple(NQueensPositionsVector & pos, Components... nQueensComponents) {
            nQueensSolutionLocalSearch(pos,
                                       utils::SkipFunctor(),
                                       utils::ReturnFalseFunctor(),
                                       std::move(nQueensComponents)...);
        }

} //!local_search
} // !paal

#endif /* N_QUEENS_LOCAL_SEARCH_HPP */
