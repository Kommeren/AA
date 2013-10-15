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
echo ------------------------------------------- shortest_superstring_long_test          -------------------------------------------------------------------------------------
./${build_dir}/test/shortest_superstring_long_test
exit_on_error
echo ------------------------------------------- 2_local_search_long_test 	 -------------------------------------------------------------------------------------
./${build_dir}/test/2_local_search_long_test --run_test=TSPLIB
exit_on_error
echo ------------------------------------------- bimap_perf_test 	 -------------------------------------------------------------------------------------
./${build_dir}/test/bimap_perf_test
exit_on_error
#echo ------------------------------------------- bounded_degree_mst_long_test 	 -------------------------------------------------------------------------------------
#./${build_dir}/bounded_degree_mst_long_test
#exit_on_error
echo ------------------------------------------- tree_augmentation_long_test     -------------------------------------------------------------------------------------
./${build_dir}/test/tree_augmentation_long_test
exit_on_error
echo ------------------------------------------- capacitated_facility_location_long_test 	 -------------------------------------------------------------------------------------
./${build_dir}/test/capacitated_facility_location_long_test --run_test=FacilityLocationLong
exit_on_error
echo ------------------------------------------- dreyfus_wagner_long_test	 -------------------------------------------------------------------------------------
./${build_dir}/test/dreyfus_wagner_long_test
exit_on_error
echo ------------------------------------------- facility_location_long_test	 -------------------------------------------------------------------------------------
./${build_dir}/test/facility_location_long_test
exit_on_error
echo ------------------------------------------- generalised_assignment_long_test 	 -------------------------------------------------------------------------------------
./${build_dir}/test/generalised_assignment_long_test
exit_on_error
echo ------------------------------------------- steiner_network_long_test 	 -------------------------------------------------------------------------------------
./${build_dir}/test/steiner_network_long_test
exit_on_error
echo -------------------------------------------zelikovsky_11_per_6_long_test 	 -------------------------------------------------------------------------------------
./${build_dir}/test/zelikovsky_11_per_6_long_test
exit_on_error
echo ------------------------------------------- k_median_long_test          -------------------------------------------------------------------------------------
./${build_dir}/test/k_median_long_test
exit_on_error
echo ------------------------------------------- scheduling_jobs_on_identical_parallel_machines_long_test      -------------------------------------------------------------------------------------
./${build_dir}/test/scheduling_jobs_on_identical_parallel_machines_long_test
exit_on_error
echo ------------------------------------------- knapsack_long_test          -------------------------------------------------------------------------------------
./${build_dir}/test/knapsack_long_test
exit_on_error
echo ------------------------------------------- knapsack_two_app_long_test  -------------------------------------------------------------------------------------
./${build_dir}/test/knapsack_two_app_long_test
exit_on_error

