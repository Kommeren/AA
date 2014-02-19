/**
 * @file facility_location_solution_element.hpp
 * @brief
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */
#ifndef FACILITY_LOCATION_SOLUTION_ELEMENT_HPP
#define FACILITY_LOCATION_SOLUTION_ELEMENT_HPP


namespace paal {
namespace local_search {
namespace facility_location {

enum is_chosen { CHOSEN, UNCHOSEN};

/**
 * @brief class representing facility consists of vertex type and  is_chosen enum value.
 *
 * @tparam T
 */
template <typename T> class Facility {
public:
    Facility(is_chosen ic, T e) : m_isChosen(ic), m_elem(e) {}
    Facility() = default;

    is_chosen getIsChosen() const {
        return m_isChosen;
    }

    void setIsChosen(is_chosen ic) {
        m_isChosen = ic;
    }

    T getElem() const {
        return m_elem;
    }

    //TODO this is to tricky
    bool operator==(const Facility & se) const {
        return /*m_isChosen == se.m_isChosen && */m_elem == se.m_elem;
    }

    template <typename ostream, typename TT>
    friend ostream & operator<<(ostream & o, Facility<TT> f) {
        o << "(" << f.getElem() << "," << f.getIsChosen() << ")";
        return o;
    }

private:
    is_chosen m_isChosen;
    T m_elem;
};

/**
 * @brief Hash for facility
 */
struct FacilityHash {
    /**
     * @brief operator()
     *
     * @tparam T
     * @param f
     *
     * @return hash
     */
    template <typename T>
    std::size_t operator()(Facility<T> f) const {
        return std::hash<T>()(f.getElem());
    }
};

/**
 * @brief operator< for facility
 *
 * @tparam T
 * @param left
 * @param right
 *
 * @return
 */
template <typename T>
bool operator<(const Facility<T> & left, const Facility<T> & right) {
    return left.getElem() < right.getElem();
}


}
}
}


#endif //FACILITY_LOCATION_SOLUTION_ELEMENT_HPP
