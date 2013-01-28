namespace paal {
namespace local_search {
namespace two_local_search {

template <typename Metric> class ImproveChecker2Opt {
    public:
        ImproveChecker2Opt(Metric & m) : m_metric(m) {}

        template <typename Solution, typename SolutionElement> int gain(Solution & s, const SolutionElement & sel, const SolutionElement &adjustEl) {
           return m_metric(sel.first, sel.second) + m_metric(adjustEl.first, adjustEl.second) - 
               m_metric(sel.first, adjustEl.first) - m_metric(sel.second, adjustEl.second);
        }

    private:
        Metric & m_metric;
};

} //two_local_search
} //local_search
} //paal
