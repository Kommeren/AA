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
./${build_dir}/2_local_search_test
exit_on_error
echo ------------------------------------------- bounded_degree_mst_test 	 -------------------------------------------------------------------------------------
./${build_dir}/bounded_degree_mst_test
exit_on_error
echo ------------------------------------------- capacitated_voronoi_test 	 -------------------------------------------------------------------------------------
./${build_dir}/capacitated_voronoi_test
exit_on_error
echo ------------------------------------------- cycle_cancellation_test 	 -------------------------------------------------------------------------------------
./${build_dir}/cycle_cancellation_test
exit_on_error
echo ------------------------------------------- facility_location_test 	 -------------------------------------------------------------------------------------
./${build_dir}/facility_location_test
exit_on_error
echo ------------------------------------------- facility_location_neghborhood_getter_test 	 -------------------------------------------------------------------------------------
./${build_dir}/facility_location_neghborhood_getter_test
exit_on_error
echo ------------------------------------------- facility_location_solution_adapter_test 	 -------------------------------------------------------------------------------------
./${build_dir}/facility_location_solution_adapter_test
exit_on_error
echo ------------------------------------------- facility_location_solution_test 	 -------------------------------------------------------------------------------------
./${build_dir}/facility_location_solution_test
exit_on_error
echo ------------------------------------------- generalised_assignment_test 	 -------------------------------------------------------------------------------------
./${build_dir}/generalised_assignment_test
exit_on_error
echo ------------------------------------------- k_median_solution_adapter_test 	 -------------------------------------------------------------------------------------
./${build_dir}/k_median_solution_adapter_test
exit_on_error
echo ------------------------------------------- k_median_test 	 -------------------------------------------------------------------------------------
./${build_dir}/k_median_test
exit_on_error
echo ------------------------------------------- local_search_multi_solution_lambdas_test 	 -------------------------------------------------------------------------------------
./${build_dir}/local_search_multi_solution_lambdas_test
exit_on_error
echo ------------------------------------------- local_search_multi_solution_test 	 -------------------------------------------------------------------------------------
./${build_dir}/local_search_multi_solution_test
exit_on_error
echo ------------------------------------------- local_search_obj_fun_test 	 -------------------------------------------------------------------------------------
./${build_dir}/local_search_obj_fun_test
exit_on_error
echo ------------------------------------------- local_search_test 	 -------------------------------------------------------------------------------------
./${build_dir}/local_search_test
exit_on_error
echo ------------------------------------------- metric_test 	 -------------------------------------------------------------------------------------
./${build_dir}/metric_test
exit_on_error
echo ------------------------------------------- path_augmentation_test 	 -------------------------------------------------------------------------------------
./${build_dir}/path_augmentation_test
exit_on_error
echo ------------------------------------------- simple_cycle_test 	 -------------------------------------------------------------------------------------
./${build_dir}/simple_cycle_test
exit_on_error
echo ------------------------------------------- splay_cycle_test 	 -------------------------------------------------------------------------------------
./${build_dir}/splay_cycle_test
exit_on_error
echo ------------------------------------------- splay_tree_test 	 -------------------------------------------------------------------------------------
./${build_dir}/splay_tree_test
exit_on_error
echo ------------------------------------------- steiner_network_test 	 -------------------------------------------------------------------------------------
./${build_dir}/steiner_network_test
exit_on_error
echo ------------------------------------------- steiner_tree_test 	 -------------------------------------------------------------------------------------
./${build_dir}/steiner_tree_test
exit_on_error
echo ------------------------------------------- subset_iterator_test 	 -------------------------------------------------------------------------------------
./${build_dir}/subset_iterator_test
exit_on_error
echo ------------------------------------------- vertex_to_edge_iterator_test 	 -------------------------------------------------------------------------------------
./${build_dir}/vertex_to_edge_iterator_test
exit_on_error
echo ------------------------------------------- voronoi_test 	 -------------------------------------------------------------------------------------
./${build_dir}/voronoi_test
exit_on_error
