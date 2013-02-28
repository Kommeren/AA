/**
 * @file steiner_tree_long_test.cpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-04
 */

#define BOOST_TEST_MODULE steiner_tree_long

#include <boost/test/unit_test.hpp>

#include <vector>
#include <fstream>

#include "utils/logger.hpp"

#include "paal/local_search/steiner_tree/steiner_tree.hpp"
#include "paal/data_structures/bimap.hpp"

#include "utils/read_steinlib.hpp"
#include "utils/sample_graph.hpp"

void readLine(std::istream & is, std::string & fname, int & OPT) {
    int dummy;
    std::string dummys;
    is >> fname >> dummy >> dummy >> dummy >> dummys >> OPT;
    fname += ".stp";
}

BOOST_AUTO_TEST_CASE(metric_to_bgl_mst_test) {
    std::string testDir = "test/data/STEINLIB/";
    std::ifstream is_test_cases(testDir + "/index");
    while(is_test_cases.good()) {
        std::string fname;
        int opt;
        readLine(is_test_cases, fname, opt);
        if(fname == ".stp")
            return;
        std::vector<int> terminals;
        std::vector<int> steinerPoints;
        LOG("TEST " << fname);
        LOG("OPT " << opt);

        std::ifstream ifs(testDir + "/I080/" + fname);
        assert(ifs.good());
        auto m = paal::readSTEINLIB(ifs, terminals, steinerPoints);
        typedef decltype(m) Metric;
        typedef paal::data_structures::Voronoi<Metric> VoronoiT;
        typedef typename VoronoiT::GeneratorsSet FSet;
        VoronoiT voronoi(FSet(terminals.begin(), terminals.end()),
                         FSet(steinerPoints.begin(), steinerPoints.end()), m);
        paal::local_search::steiner_tree::SteinerTree<Metric, VoronoiT> st(m, voronoi);
        std::vector<int> selectedSteinerPoints = st.getResultSteinerVertices(); 

        auto resRange = boost::join(terminals, selectedSteinerPoints);
        LOG_COPY_DEL(boost::begin(resRange), boost::end(resRange), ",");
        paal::data_structures::BiMap<int> idx;
        auto g = paal::utils::metricToBGLWithIndex(m, boost::begin(resRange), boost::end(resRange), idx);
        std::vector<int> pm(resRange.size());
        boost::prim_minimum_spanning_tree(g, &pm[0]);
        paal::utils::MetricOnIdx<Metric> idxM(m, idx);
        int res(0);
        for(int i : boost::irange(0, int(pm.size()))) {
            res += idxM(i, pm[i]);
        }
        LOG("RES " << res);

        LOG("APPROXIMATION_RATIO:" << double(res) / double(opt));
    }

   //BOOST_CHECK_EQUAL(s, 6);
}
