/**
 * @file bound_type.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-05-06
 */
#ifndef BOUND_TYPE_HPP
#define BOUND_TYPE_HPP

namespace paal {
namespace lp {
// free, only lower bound, only upper bound, both bounds, fix
enum BoundType {FR, LO, UP, DB, FX};
}
}
#endif /* BOUND_TYPE_HPP */
