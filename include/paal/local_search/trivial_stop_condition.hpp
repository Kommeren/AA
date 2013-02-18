/**
 * @file trivial_stop_condition.hpp
 * @brief trivial stop condition
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-04
 */
#ifndef TRIVIAL_STOP_CONDITION_HPP
#define TRIVIAL_STOP_CONDITION_HPP
    

namespace paal {
namespace local_search {

/**
 * @brief 
 */
class TrivialStopCondition {
public:
    /**
     * @brief trivial stop condition
     *
     *  not used
     *  not used
     *
     * @return always true
     */
    template <typename Solution, typename Update>
    bool stop(const Solution &, const Update &) {
        return false;
    }
};

}
}

#endif /* TRIVIAL_STOP_CONDITION_HPP */
