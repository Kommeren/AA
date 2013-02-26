#define BOOST_TEST_MODULE two_local_search_long

#include <boost/test/unit_test.hpp>

#include <vector>
#include <string>

#include "utils/read_tsplib.h"
#include "paal/local_search/2_local_search/2_local_search.hpp"
#include "paal/simple_algo/cycle_algo.hpp"
#include "utils/logger.hpp"
#include "paal/data_structures/simple_cycle.hpp"

using std::string;
using std::vector;
using namespace  paal::local_search::two_local_search;
using namespace  paal;


std::string path = "test/data/TSPLIB/symmetrical/";


BOOST_AUTO_TEST_CASE(TSPLIB) {
   read_tsplib::TSPLIB_Directory dir(path);
   read_tsplib::TSPLIB_Matrix mtx;
   for(auto &g : dir.graphs)  {   
        g.load(mtx);
        auto size = mtx.size1();
        std::vector<int> v(size);
        std::iota(v.begin(), v.end(), 0);

        //create random solution 
        std::random_shuffle(v.begin(), v.end());
        data_structures::SimpleCycleStartFromLastChange<int> cycle(v.begin(), v.end());

        //creating local search
        auto lsc = getDefaultTwoLocalComponents(mtx);
        auto ls = TwoLocalSearchStep<decltype(cycle), decltype(lsc)>(std::move(cycle), std::move(lsc));

        //printing 

#ifdef LOGGER_ON
        auto const & cman = ls.getSolution();
        LOG("Graph:\t" << g.filename);
        LOG("Length before\t" << simple_algo::getLength(mtx, cman));
        int i = 0;
#endif

        //search
        while(ls.search()) {
            LOG("Length after\t" << i++ << ": " << simple_algo::getLength(mtx, cman));
        }
   }
}

template <typename ImproveChecker, typename Dist>
class ImproveCheckerCutSmallImproves {

public:    
    ImproveCheckerCutSmallImproves(ImproveChecker ic, Dist currOpt, double epsilon) :
                m_improveChecker(std::move(ic)), m_currOpt(currOpt), m_epsilon(epsilon)  {}
    template <typename... Args> Dist 
        gain(Args&&... args) { 
        Dist dist = m_improveChecker.gain(std::forward<Args>(args)...);
        if(dist > m_epsilon * m_currOpt) {
            m_currOpt -= dist;
            return dist;
        }
        return 0;
    }

    void setEpsilon(double e) {
        m_epsilon = e;
    }


private:
    ImproveChecker m_improveChecker;
    Dist m_currOpt;    
    double m_epsilon;    
};

using paal::local_search::SearchComponents;

template <typename SearchComponents, typename NewImproveChecker> 
struct SwapImproveChecker {};

template <typename NeighborhoodGetter, 
          typename ImproveChecker, 
          typename SolutionUpdater,
          typename StopCondition,
          typename NewImproveChecker> 
struct SwapImproveChecker<SearchComponents<NeighborhoodGetter, ImproveChecker, SolutionUpdater, StopCondition>, NewImproveChecker> {
    typedef SearchComponents<NeighborhoodGetter, NewImproveChecker, SolutionUpdater, StopCondition> type;
};

template <typename NeighborhoodGetter, 
          typename ImproveChecker, 
          typename SolutionUpdater,
          typename StopCondition,
          typename NewImproveChecker> 
SearchComponents<NeighborhoodGetter, NewImproveChecker, SolutionUpdater, StopCondition> 
swapImproveChecker(SearchComponents<NeighborhoodGetter, ImproveChecker, SolutionUpdater, StopCondition> sc, 
                   NewImproveChecker ic) {
    return make_SearchComponents(std::move(sc.getNeighborhoodGetter()), 
                                 std::move(ic), 
                                 std::move(sc.getSolutionUpdater()),
                                 std::move(sc.getStopCondition()));
}


BOOST_AUTO_TEST_CASE(TSPLIB_long) {
    const string indexFName = "index.long";
    read_tsplib::TSPLIB_Directory dir(path + "/long/", indexFName);
    read_tsplib::TSPLIB_Matrix mtx;
    for(auto &g : dir.graphs)  {   
        g.load(mtx);
        auto size = mtx.size1();
        std::vector<int> v(size);
        std::iota(v.begin(), v.end(), 0);

        //create random solution 
        std::random_shuffle(v.begin(), v.end());
        data_structures::SimpleCycleStartFromLastChange<int> cycle(v.begin(), v.end());
        int startLen = simple_algo::getLength(mtx, cycle);

        //creating local search
        auto lsc = getDefaultTwoLocalComponents(mtx);
        typedef ImproveCheckerCutSmallImproves<puretype(lsc.getImproveChecker()), int> CIC;
        double epsilon = 0.001;
        CIC  cut(std::move(lsc.getImproveChecker()), startLen, epsilon);
        auto cutLsc = swapImproveChecker(lsc, std::move(cut));
        auto lsCut = TwoLocalSearchStep<decltype(cycle), decltype(cutLsc)>(std::move(cycle), std::move(cutLsc));

        //printing 

#ifdef LOGGER_ON
        auto const & cman = lsCut.getSolution();
        LOG("Graph:\t" << g.filename);
        LOG("Length before\t" << simple_algo::getLength(mtx, cman));
        int i = 0;
#endif

        //search
        for(int j = 0; j < 20; ++j) {
            epsilon /= 2;
            LOG("epsilon = " << epsilon);
            lsCut.getSearchComponents().getImproveChecker().setEpsilon(epsilon);
            while(lsCut.search()) {
                LOG("Length after\t" << i++ << ": " << simple_algo::getLength(mtx, cman));
            }
        }
        
        auto ls = TwoLocalSearchStep<decltype(cycle), decltype(lsc)>(std::move(cycle), std::move(lsc));
        lsCut.getSearchComponents().getImproveChecker().setEpsilon(epsilon);
        while(lsCut.search()) {
                LOG("Length after\t" << i++ << ": " << simple_algo::getLength(mtx, cman));
        }
    }
}

