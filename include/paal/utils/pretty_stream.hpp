/**
 * @file pretty_stream.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2014-06-12
 */
#ifndef PRITTY_STREAM_HPP
#define PRITTY_STREAM_HPP

#include <iostream>

namespace paal {

/**
 * @brief pretty_to_string prints double which is close to int as int
 *
 * @param x
 * @param epsilon
 *
 * @return
 */
inline std::string pretty_to_string(double x, double epsilon = 1e-9) {
    int r = std::round(x);
    if (utils::compare<double>{
        epsilon
    }.e(x, r)) {
        return std::to_string(r);
    } else {
        return std::to_string(x);
    }
}

/**
 * @brief generic version of pretty_to_string
 *
 * @tparam T
 * @param t
 *
 * @return
 */
template <typename T> std::string pretty_to_string(T &&t) {
    return std::to_string(t);
}

/**
 * @brief pretty_stream stream that uses pretty_to_string method
 *
 * @tparam Stream
 */
template <typename Stream> struct pretty_stream {
    /**
     * @brief constructor
     *
     * @param stream
     * @param epsilon
     */
    pretty_stream(Stream &stream, double epsilon = 1e-9)
        :
          // TODO curly braces doesn't work here (gcc compiles error)
          m_stream(stream),
          m_epsilon{ epsilon } {}

    /**
     * @brief operator<< for double
     *
     * @param x
     *
     * @return
     */
    inline std::string pretty_to_string(double x, double epsilon = 1e-9) {
        int r = std::round(x);
        if (utils::compare<double>{
            epsilon
        }.e(x, r)) {
            return std::to_string(r);
        } else {
            return std::to_string(x);
        }
    }

    /**
     * @brief operator<< generic
     *
     * @tparam T
     * @param t
     *
     * @return
     */
    template <typename T> pretty_stream<Stream> &operator<<(T &&t) {
        m_stream << t;
        return *this;
    }

    using Manipulator = Stream & (Stream &);

    /**
     * @brief operator<< for manipulators
     *
     * @param m
     *
     * @return
     */
    pretty_stream<Stream> &operator<<(Manipulator m) {
        m_stream << m;
        return *this;
    }

  private:

    Stream &m_stream;
    double m_epsilon;
};

/**
 * @brief make for pretty_stream
 *
 * @tparam Stream
 * @param s
 * @param epsilon
 *
 * @return
 */
template <typename Stream>
pretty_stream<Stream> make_pretty_stream(Stream &s, double epsilon = 1e-9) {
    return pretty_stream<Stream>(s, epsilon);
}

}      //!paal
#endif /* PRITTY_STREAM_HPP */