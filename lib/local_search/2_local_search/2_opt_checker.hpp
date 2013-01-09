
template <typename Metric> class CheckIfImprove2Opt {
    public:
        CheckIfImprove2Opt(Metric m) : m_metric(m) {}

        template <typename SolutionRange, typename SolutionElement> bool checkIfImproved(const SolutionElement & sel, const SolutionRange & sRange, const SolutionElement &adjustEl) {
           return m_metric(sel.first, sel.second) + m_metric(adjustEl.first, adjustEl.second) > 
               m_metric(sel.first, adjustEl.first) + m_metric(sel.second, adjustEl.second);
        }

    private:
        Metric m_metric;
};
