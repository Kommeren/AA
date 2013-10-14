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

template <typename Metric> class GainToOpt {
    public:
        GainToOpt(const Metric & m) : m_metric(m) {}

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
