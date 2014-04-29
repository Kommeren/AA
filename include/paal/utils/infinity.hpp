/**
 * @file infinity.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2014-04-29
 */
#ifndef INFINITY_HPP
#define INFINITY_HPP

namespace paal {

    /**
     * @brief smaller then everything
     */
struct minus_infinity {
    ///operator<
    template <typename T>
    bool operator<( const T &) const {
        return true;
    }

    ///operator>
    template <typename T>
    bool operator>( const T &) const {
        return false;
    }

    ///operator<=
    template <typename T>
    bool operator<=( const T &) const {
        return true;
    }

    ///operator>=
    template <typename T>
    bool operator>=( const T &) const {
        return false;
    }

    ///friend operator<
    template <typename T>
    friend bool operator<( const T &, minus_infinity) {
        return false;
    }

    ///friend operator>
    template <typename T>
    friend bool operator>( const T &, minus_infinity) {
        return true;
    }

    ///friend operator<=
    template <typename T>
    friend bool operator<=( const T &, minus_infinity) {
        return false;
    }

    ///friend operator>=
    template <typename T>
    friend bool operator>=( const T &, minus_infinity) {
        return true;
    }
};


/**
 * @brief bigger then everything
 */
struct plus_infinity {
    ///operator<
    template <typename T>
    bool operator<( const T &) const {
        return false;
    }

    ///operator>
    template <typename T>
    bool operator>( const T &) const {
        return true;
    }

    ///operator<=
    template <typename T>
    bool operator<=( const T &) const {
        return false;
    }

    ///operator>=
    template <typename T>
    bool operator>=( const T &) const {
        return true;
    }

    ///friend operator<
    template <typename T>
    friend bool operator<( const T &, plus_infinity) {
        return true;
    }

    ///friend operator>
    template <typename T>
    friend bool operator>( const T &, plus_infinity) {
        return false;
    }

    ///friend operator<=
    template <typename T>
    friend bool operator<=( const T &, plus_infinity) {
        return true;
    }

    ///friend operator>=
    template <typename T>
    friend bool operator>=( const T &, plus_infinity) {
        return false;
    }
};

}//!paal


#endif /* INFINITY_HPP */
