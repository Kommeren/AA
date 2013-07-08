if [[ $# != 1 ]]; then
echo help:
echo "$0 <build_dir>"
exit
fi
build_dir=$1
echo ------------------------------------------- 2_local_search_long_test 	 -------------------------------------------------------------------------------------
./${build_dir}/2_local_search_long_test --run_test=TSPLIB
echo ------------------------------------------- bimap_perf_test 	 -------------------------------------------------------------------------------------
./${build_dir}/bimap_perf_test
#echo ------------------------------------------- bounded_degree_mst_long_test 	 -------------------------------------------------------------------------------------
#./${build_dir}/bounded_degree_mst_long_test
echo ------------------------------------------- capacitated_facility_location_long_test 	 -------------------------------------------------------------------------------------
./${build_dir}/capacitated_facility_location_long_test --run_test=FacilityLocationLong
echo ------------------------------------------- facility_location_long_test 	 -------------------------------------------------------------------------------------
./${build_dir}/facility_location_long_test
echo ------------------------------------------- generalised_assignment_long_test 	 -------------------------------------------------------------------------------------
./${build_dir}/generalised_assignment_long_test
echo ------------------------------------------- steiner_network_long_test 	 -------------------------------------------------------------------------------------
./${build_dir}/steiner_network_long_test
echo ------------------------------------------- steiner_tree_long_test 	 -------------------------------------------------------------------------------------
./${build_dir}/steiner_tree_long_test
