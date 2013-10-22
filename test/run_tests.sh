if [[ $# != 1 ]]; then
    echo help:
    echo "$0 <build_dir>"
    exit
fi

exit_on_error() {
    if [[ $? != 0 ]]; then
        echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ERROR !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
        exit
    fi
}

tests="2_local_search_test bounded_degree_mst_test capacitated_voronoi_test cycle_canceling_test dreyfus_wagner_test facility_location_test
       facility_location_neghborhood_getter_test facility_location_solution_adapter_test facility_location_solution_test generalised_assignment_test
       k_median_test k_median_solution_adapter_test local_search_multi_solution_test local_search_multi_solution_lambdas_test local_search_test
       local_search_obj_fun_test metric_test successive_shortest_path_test simple_cycle_test splay_tree_test splay_cycle_test steiner_network_test
       zelikovsky_11_per_6_test subset_iterator_test vertex_to_edge_iterator_test voronoi_test tree_augmentation_test 
       scheduling_jobs_with_deadlines_on_a_single_machine_test knapsack_test scheduling_jobs_on_identical_parallel_machines_test k_cut_test"

option() {
    ret=""
    case $1 in
        *) 
            ;;
    esac
    $ret
}

build_dir=$1

for t in $tests; do
    echo ------------------------------------------- $t 	 -------------------------------------------------------------------------------------
    ./${build_dir}/test/$t $(option $t)
    exit_on_error
done
