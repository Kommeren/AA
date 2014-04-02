if [ $# -lt 1 -o $# -gt 2 ];
then
    echo "usage: ./run_compilation_error_tests.sh build_directory [test_name]"
    echo "Before running the script you need to run cmake with option -DCOMPILATION_ERROR_TESTS=1 in build_directory"
    exit 1
fi

TESTS=$(find . -name "*compilation_error_test*.cpp" -printf "%f\n" | sed s/.cpp//g)

cd $1
test_found=0
for test_name in $TESTS;
do
    if [ $# -eq 1 ] || [ $test_name == $2 ];
    then
        test_found=1
        make $test_name
        if [ $? -eq 0 ]
        then
            echo "ERROR: Test $test_name should not compile"
            exit 1
        fi
    fi
done

if [ $test_found -eq 1 ];
then
    echo "OK: All tests failed to compile"
    exit 0
else
    echo "Test $2 not found"
    exit 1
fi
