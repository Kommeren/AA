/**
 * @file logger.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-13
 */
#ifndef LOGGER_HPP
#define LOGGER_HPP

//#define LOGGER_ON

#ifdef LOGGER_ON
#       include <iostream>
#       include "helpers/type_functions.hpp"
#       define LOG(x) std::cout<< x <<std::endl
#       define LOG_COPY(b, e) \
            std::copy(b, e, std::ostream_iterator<typename paal::helpers::IterToElem<decltype(b)>::type>(std::cout, "\n"));\
            LOG("")
#else
#       define LOG(x)
#       define LOG_COPY(b, e)
#endif


#endif /* LOGGER_HPP */
