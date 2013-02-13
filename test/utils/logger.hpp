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
#include <iostream>
#define LOG(x) std::cout<< x <<std::endl
#else
#define LOG(x)
#endif


#endif /* LOGGER_HPP */
