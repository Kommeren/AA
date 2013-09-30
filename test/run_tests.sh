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

build_dir=$1
echo ------------------------------------------- 2_local_search_test 	 -------------------------------------------------------------------------------------
./${build_dir}/test/2_local_search_test
exit_on_error
echo ------------------------------------------- bounded_degree_mst_test 	 -------------------------------------------------------------------------------------
./${build_dir}/test/bounded_degree_mst_test
exit_on_error
echo ------------------------------------------- capacitated_voronoi_test 	 -------------------------------------------------------------------------------------
./${build_dir}/test/capacitated_voronoi_test
exit_on_error
echo ------------------------------------------- cycle_canceling_test 	 -------------------------------------------------------------------------------------
./${build_dir}/test/cycle_canceling_test
exit_on_error
echo ------------------------------------------- dreyfus_wagner_test 	 -------------------------------------------------------------------------------------
./${build_dir}/test/dreyfus_wagner_test
exit_on_error
echo ------------------------------------------- facility_location_test 	 -------------------------------------------------------------------------------------
./${build_dir}/test/facility_location_test
exit_on_error
echo ------------------------------------------- facility_location_neghborhood_getter_test 	 -------------------------------------------------------------------------------------
./${build_dir}/test/facility_location_neghborhood_getter_test
exit_on_error
echo ------------------------------------------- facility_location_solution_adapter_test 	 -------------------------------------------------------------------------------------
./${build_dir}/test/facility_location_solution_adapter_test
exit_on_error
echo ------------------------------------------- facility_location_solution_test 	 -------------------------------------------------------------------------------------
./${build_dir}/test/facility_location_solution_test
exit_on_error
echo ------------------------------------------- generalised_assignment_test 	 -------------------------------------------------------------------------------------
./${build_dir}/test/generalised_assignment_test
exit_on_error
echo ------------------------------------------- k_median_solution_adapter_test 	 -------------------------------------------------------------------------------------
./${build_dir}/test/k_median_solution_adapter_test
exit_on_error
echo ------------------------------------------- k_median_test 	 -------------------------------------------------------------------------------------
./${build_dir}/test/k_median_test
exit_on_error
echo ------------------------------------------- local_search_multi_solution_lambdas_test 	 -------------------------------------------------------------------------------------
./${build_dir}/test/local_search_multi_solution_lambdas_test
exit_on_error
echo ------------------------------------------- local_search_multi_solution_test 	 -------------------------------------------------------------------------------------
./${build_dir}/test/local_search_multi_solution_test
exit_on_error
echo ------------------------------------------- local_search_obj_fun_test 	 -------------------------------------------------------------------------------------
./${build_dir}/test/local_search_obj_fun_test
exit_on_error
echo ------------------------------------------- local_search_test 	 -------------------------------------------------------------------------------------
./${build_dir}/test/local_search_test
exit_on_error
echo ------------------------------------------- metric_test 	 -------------------------------------------------------------------------------------
./${build_dir}/test/metric_test
exit_on_error
echo ------------------------------------------- successive_shortest_path_test 	 -------------------------------------------------------------------------------------
./${build_dir}/test/successive_shortest_path_test
exit_on_error
echo ------------------------------------------- simple_cycle_test 	 -------------------------------------------------------------------------------------
./${build_dir}/test/simple_cycle_test
exit_on_error
echo ------------------------------------------- splay_cycle_test 	 -------------------------------------------------------------------------------------
./${build_dir}/test/splay_cycle_test
exit_on_error
echo ------------------------------------------- splay_tree_test 	 -------------------------------------------------------------------------------------
./${build_dir}/test/splay_tree_test
exit_on_error
echo ------------------------------------------- steiner_network_test 	 -------------------------------------------------------------------------------------
./${build_dir}/test/steiner_network_test
exit_on_error
echo ------------------------------------------- zelikovsky_11_per_6_test 	 -------------------------------------------------------------------------------------
./${build_dir}/test/zelikovsky_11_per_6_test
exit_on_error
echo ------------------------------------------- subset_iterator_test 	 -------------------------------------------------------------------------------------
./${build_dir}/test/subset_iterator_test
exit_on_error
echo ------------------------------------------- vertex_to_edge_iterator_test 	 -------------------------------------------------------------------------------------
./${build_dir}/test/vertex_to_edge_iterator_test
exit_on_error
echo ------------------------------------------- voronoi_test 	 -------------------------------------------------------------------------------------
./${build_dir}/test/voronoi_test
exit_on_error
echo ------------------------------------------- tree_augmentation_test 	 -------------------------------------------------------------------------------------
./${build_dir}/test/tree_augmentation_test
exit_on_error
echo ------------------------------------------- scheduling_jobs_on_identical_parallel_machines_test      -------------------------------------------------------------------------------------
./${build_dir}/test/scheduling_jobs_on_identical_parallel_machines_test
exit_on_error
echo ------------------------------------------- knapsack_test 	 -------------------------------------------------------------------------------------
./${build_dir}/test/knapsack_test
exit_on_error
echo ------------------------------------------- scheduling_jobs_with_deadlines_on_a_single_machine_test      -------------------------------------------------------------------------------------
./${build_dir}/test/scheduling_jobs_with_deadlines_on_a_single_machine_test
exit_on_error
echo ------------------------------------------- k_cut_test      -------------------------------------------------------------------------------------
./${build_dir}/test/k_cut_test
exit_on_error
