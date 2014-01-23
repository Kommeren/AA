/**
 * @file in_balls.cpp
 * @brief
 * @author Piotr Smulewicz
 * @version 1.0
 * @date 2013-01-29
 */

#ifndef IN_BALLS_HPP
#define IN_BALLS_HPP

#include <boost/test/unit_test.hpp>

namespace paal {

template<typename IT,typename CE, typename Metric,typename Dist>
void inBalls(const IT& items,const CE& centers,const Metric& metric,Dist radious){
    for(auto j:items){
        bool inBall=false;
        for(auto i:centers){
            if(metric(j,i)<=radious){
                inBall=true;
                break;
            }
        }
        BOOST_CHECK(inBall);
    }
}
}//paal
#endif /* IN_BALLS_HPP */