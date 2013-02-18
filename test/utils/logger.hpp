/**
 * @file logger.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-13
 */
#ifndef LOGGER_HPP
#define LOGGER_HPP

#define LOGGER_ON

#ifdef LOGGER_ON
#       include <iostream>
#       include "paal/helpers/type_functions.hpp"
#       define LOG(x) std::cout<< x <<std::endl
#       define LOG_COPY(b, e) \
            std::copy(b, e, std::ostream_iterator<typename paal::helpers::IterToElem<decltype(b)>::type>(std::cout, "\n"));\
            LOG("")
#       define LOG_COPY_DEL(b, e, del) \
            std::copy(b, e, std::ostream_iterator<typename paal::helpers::IterToElem<decltype(b)>::type>(std::cout, del));\
            LOG("")
#else
#       define LOG(x)
#       define LOG_COPY(b, e)
#       define LOG_COPY_DEL(b, e, del)
#endif


#endif /* LOGGER_HPP */
