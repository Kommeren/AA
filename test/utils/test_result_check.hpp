/**
 * @file test_result_check.hpp
 * @brief
 * @author Piotr Smulewicz
 * @version 1.0
 * @date 2014-01-29
 */
#ifndef TEST_RESULT_CHECK_HPP
#define TEST_RESULT_CHECK_HPP
#include <boost/test/unit_test.hpp>

#include "utils/logger.hpp"

#include "paal/utils/functors.hpp"

#include <iomanip>

template<typename T,typename T2,typename Comparator=paal::utils::less_equal>
void check_result(T result,T optimal,T2 aproximation_ratio,Comparator comp = Comparator(),
                  T eps=0,
                  std::string opt="optimal: ",
                  std::string apr_ratio="aproximation ratio: "){
    LOGLN("result: "<<std::setprecision(20)<<result);
    LOGLN(opt<<optimal);
    LOGLN(apr_ratio<<double(result)/optimal);
    // +eps is for comp =Less() , -eps if for comp =Greater().
    BOOST_CHECK(comp(optimal,result+eps)||comp(optimal,result-eps));
    BOOST_CHECK(comp(double(result)+eps,double(optimal)*aproximation_ratio)||
                comp(double(result)-eps,double(optimal)*aproximation_ratio)
    );
};

template<typename T,typename T2,typename Comaparator=paal::utils::less_equal>
void check_result_compare_to_bound(T result,T upper_bound_for_optimal,T2 aproximation_ratio,Comaparator comp = paal::utils::less_equal(),
                  T eps=0,
                  std::string ub="Solution is not better than: ",
                  std::string apr_ratio="Aproxmimation ratio is not worst than: "){
    LOGLN(std::setprecision(20)<<"result: "<<result);
    if(upper_bound_for_optimal!=0){
        LOGLN(ub<<upper_bound_for_optimal);
        // +eps is for comp =Less() , -eps if for comp =Greater().
        LOGLN(apr_ratio<<double(result)/upper_bound_for_optimal);
        BOOST_CHECK(comp(double(result)+eps,double(upper_bound_for_optimal)*aproximation_ratio)||
                    comp(double(result)-eps,double(upper_bound_for_optimal)*aproximation_ratio));
    }
};

#endif /* TEST_RESULT_CHECK_HPP */
