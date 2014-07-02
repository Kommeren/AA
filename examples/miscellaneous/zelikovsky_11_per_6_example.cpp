/**
 * @file zelikovsky_11_per_6_example.cpp
 * @brief This is example for zelikovsky 11/6 approximation for steiner tree
 * problem.
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-04
 */
    //! [Steiner Tree Example]
#include "paal/steiner_tree/zelikovsky_11_per_6.hpp"

#include "test/utils/sample_graph.hpp"

#include <iostream>

int main() {
    // sample metric
    typedef sample_graphs_metrics SGM;
    auto gm = SGM::get_graph_metric_steiner();
    typedef decltype(gm) Metric;

    // sample voronoi
    typedef paal::data_structures::voronoi<Metric> voronoiT;
    typedef paal::data_structures::voronoi_traits<voronoiT> VT;
    typedef typename VT::GeneratorsSet GSet;
    typedef typename VT::VerticesSet VSet;
    voronoiT voronoi(GSet{ SGM::A, SGM::B, SGM::C, SGM::D }, VSet{ SGM::E },
                     gm);

    // run algorithm
    std::vector<int> steiner_points;
    paal::steiner_tree_zelikovsky11per6approximation(
        gm, voronoi, std::back_inserter(steiner_points));

    // print result
    std::cout << "Steiner points:" << std::endl;
    std::copy(steiner_points.begin(), steiner_points.end(),
              std::ostream_iterator<int>(std::cout, "\n"));
    return 0;
}
    //! [Steiner Tree Example]
