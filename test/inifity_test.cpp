/**
 * @file infinity_test.cpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-06-20
 */
#include <boost/test/unit_test.hpp>


#include "paal/utils/infinity.hpp"
class A{};

BOOST_AUTO_TEST_CASE(inifity_test) {
    const A a{};
    const paal::plus_infinity p_inf{};
    const paal::minus_infinity m_inf{};

    //comparing A and plus_infinity
    BOOST_CHECK(  a <  p_inf);
    BOOST_CHECK(  a <= p_inf);
    BOOST_CHECK(!(a >  p_inf));
    BOOST_CHECK(!(a >= p_inf));
    BOOST_CHECK(  p_inf >  a);
    BOOST_CHECK(  p_inf >= a);
    BOOST_CHECK(!(p_inf <  a));
    BOOST_CHECK(!(p_inf <= a));

    //comparing plus_infinity and plus_infinity
    BOOST_CHECK(!(p_inf <  p_inf));
    BOOST_CHECK(  p_inf <= p_inf );
    BOOST_CHECK(!(p_inf >  p_inf));
    BOOST_CHECK(  p_inf >= p_inf );

    //comparing A and minus_infinity
    BOOST_CHECK(  a >  m_inf);
    BOOST_CHECK(  a >= m_inf);
    BOOST_CHECK(!(a <  m_inf));
    BOOST_CHECK(!(a <= m_inf));
    BOOST_CHECK(  m_inf <  a);
    BOOST_CHECK(  m_inf <= a);
    BOOST_CHECK(!(m_inf >  a));
    BOOST_CHECK(!(m_inf >= a));

    //comparing minus_infinity and minus_infinity
    BOOST_CHECK(!(m_inf <  m_inf));
    BOOST_CHECK(  m_inf <= m_inf );
    BOOST_CHECK(!(m_inf >  m_inf));
    BOOST_CHECK(  m_inf >= m_inf );

    //comparing minus_infinity and plus_infinity
    BOOST_CHECK(  p_inf >  m_inf);
    BOOST_CHECK(  p_inf >= m_inf);
    BOOST_CHECK(!(p_inf <  m_inf));
    BOOST_CHECK(!(p_inf <= m_inf));
    BOOST_CHECK(  m_inf <  p_inf);
    BOOST_CHECK(  m_inf <= p_inf);
    BOOST_CHECK(!(m_inf >  p_inf));
    BOOST_CHECK(!(m_inf >= p_inf));
}


