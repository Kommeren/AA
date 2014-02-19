/**
 * @file 2_local_search_checker.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */
namespace paal {
namespace local_search {
namespace two_local_search {

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
         * @param s
         * @param sel
         * @param adjustEl
         *
         * @return
         */
        template <typename Solution, typename SolutionElement>
        int operator()(const Solution & s, const SolutionElement & sel, const SolutionElement &adjustEl) {
           return m_metric(sel.first, sel.second) + m_metric(adjustEl.first, adjustEl.second) -
               m_metric(sel.first, adjustEl.first) - m_metric(sel.second, adjustEl.second);
        }

    private:
        const Metric & m_metric;
};

} //two_local_search
} //local_search
} //paal
