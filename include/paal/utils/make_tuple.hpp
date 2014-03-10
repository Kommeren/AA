/**
 * @file make_tuple.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2014-03-10
 */
#ifndef MAKE_TUPLE_HPP
#define MAKE_TUPLE_HPP

namespace paal {
/**
 * @brief function object  for std::make_tuple
 */
struct MakeTuple {
    /**
     * @brief operator()
     *
     * @tparam Args
     *
     * @return
     */
    template <typename... Args>
        auto operator()(Args&&...args) const ->
        decltype(std::make_tuple(std::forward<Args>(args)...)) {
            return std::make_tuple(std::forward<Args>(args)...);
        }
};

}//!paal

#endif /* MAKE_TUPLE_HPP */
