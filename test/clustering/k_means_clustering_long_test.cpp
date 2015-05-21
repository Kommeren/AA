/**
 * @file k_means_clustering_long_test.cpp
 * @brief
 * @author Piotr Smulewicz
 * @version 1.0
 * @date 2014-09-14
 */

#include "test_utils/logger.hpp"
#include "test_utils/read_two_dimensional_data.hpp"
#include "test_utils/test_result_check.hpp"
#include "test_utils/get_test_dir.hpp"
#include "test_utils/system.hpp"

#include "paal/clustering/k_means_clustering.hpp"
#include "paal/utils/parse_file.hpp"
#include "paal/utils/type_functions.hpp"

#include <boost/range/algorithm/max_element.hpp>

#include <iostream>
#include <algorithm>

const bool PRINT_SVG = false;

inline auto scale(double e, double max_value) { return e / max_value * 1e3; };

template <class Stream> struct print_svg_visitor : public paal::k_means_visitor {
    print_svg_visitor(Stream &stream, double max_value) : m_stream(stream), m_max_value(max_value) {};
    template <class Center, class New_center>
    void move_center(Center &&last_center, New_center &&new_center) {
        m_stream << "<line x1=\"" << scale(last_center[0], m_max_value)
                 <<    "\" y1=\"" << scale(last_center[1], m_max_value)
                 <<    "\" x2=\"" << scale(new_center[0], m_max_value)
                 <<    "\" y2=\"" << scale(new_center[1], m_max_value)
                 << "\" style=\"stroke:rgb(255,0,0);stroke-width:1\" />\n";
    }

  private:

    Stream &m_stream;
    double m_max_value;
};
BOOST_AUTO_TEST_CASE(k_means_clustering_long_test) {
    using Point = std::vector<double>;

    std::string test_dir = paal::system::get_test_data_dir("CLUSTERING");
    using paal::system::build_path;
    paal::parse(build_path(test_dir, "index"),
                [&](const std::string &fname, std::istream &is_test_cases) {

        LOGLN("TEST " << fname);
        int number_of_clusters;
        is_test_cases >> number_of_clusters;
        std::ifstream ifs(build_path(test_dir, "/cases/" + fname + ".txt"));
        assert(ifs.good());

        auto points = paal::read_two_dimensional_data<>(ifs);

        std::vector<Point> start_centers;
        paal::get_random_centers(points,number_of_clusters,back_inserter(start_centers));

        std::vector<std::pair<Point, int>> point_cluster_pair;

        double max_value=0;
        for(auto point : points){
            max_value = std::max(max_value, *boost::max_element(point));
        }

        if(!PRINT_SVG) {
            auto centers=paal::k_means(points, start_centers,
                back_inserter(point_cluster_pair));
            return;
        }

        std::ofstream ofs (fname+".svg", std::ofstream::out);
        ofs << "<svg width=\"1000\" height=\"1000\">\n" <<
                "<circle cx=\"0\" cy=\"0\" r=\"2000\" stroke=\"green\" stroke-width=\"0\" fill=\"black\" />\n";
        auto centers=paal::k_means(points, start_centers,
                back_inserter(point_cluster_pair), print_svg_visitor<std::ofstream>(ofs, max_value));


        std::vector<std::string> colors = {
                                    "brown",
                                    "burlywood",
                                    "cadetblue",
                                    "chartreuse",
                                    "chocolate",
                                    "coral",
                                    "cornflowerblue",
                                    "cornsilk",
                                    "crimson",
                                    "darkblue",
                                    "darkcyan",
                                    "darkgoldenrod"
                                };

        auto scale_1 = [=](double d){ return scale(d, max_value); };
        for (auto i : point_cluster_pair) {
            ofs
                << "<circle cx=\"" << scale_1(i.first[0])
                << "\" cy=\""      << scale_1(i.first[1])
                << "\" r=\"1\" stroke=\"green\" stroke-width=\"0\" fill=\""
                << colors[i.second%colors.size()] << "\" />"
                << "\n";
        }
        for (auto i : centers) {
            if(i.size()>1){
            ofs
                << "<circle cx=\"" << scale_1(i[0])
                << "\" cy=\""      << scale_1(i[1])
                << "\" r=\"5\" stroke=\"green\" stroke-width=\"0\" fill=\""
                << "red" << "\" />"
                << "\n";
                }
        }
        ofs << "</svg>\n";
        ofs.close();
    });

}
