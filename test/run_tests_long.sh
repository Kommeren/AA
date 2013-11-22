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

option() {
    ret=""
    case $1 in
        "2_local_search_long_test")
            ret="--run_test=TSPLIB"
            ;;
        "capacitated_facility_location_long_test")
            ret="--run_test=FacilityLocationLong"
            ;;
        *) 
            ;;
    esac
    echo $ret
}

tests="shortest_superstring_long_test 2_local_search_long_test bimap_perf_long_test bounded_degree_mst_long_test tree_augmentation_long_test 
       capacitated_facility_location_long_test dreyfus_wagner_long_test facility_location_long_test generalised_assignment_long_test ir_steiner_tree_long_test
       steiner_network_long_test zelikovsky_11_per_6_long_test k_median_long_test scheduling_jobs_on_identical_parallel_machines_long_test
       knapsack_two_app_long_test knapsack_long_test k_cut_long_test"

build_dir=$1

for t in $tests; do
    echo ------------------------------------------- $t 	 -------------------------------------------------------------------------------------
    ./${build_dir}/test/$t $(option $t)
    exit_on_error
done
