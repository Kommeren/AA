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

    /**
     * @brief enum indicates if facility is chosen (open), or not
     */
enum is_chosen { CHOSEN, UNCHOSEN};

/**
 * @brief class representing facility consists of vertex type and  is_chosen enum value.
 *
 * @tparam T
 */
template <typename T> class Facility {
public:
    /**
     * @brief constructor
     *
     * @param ic
     * @param e
     */
    Facility(is_chosen ic, T e) : m_isChosen(ic), m_elem(e) {}
    Facility() = default;

    /**
     * @brief gettr for is_chosen
     *
     * @return
     */
    is_chosen getIsChosen() const {
        return m_isChosen;
    }

    /**
     * @brief setter for is_chosen
     *
     * @param ic
     */
    void setIsChosen(is_chosen ic) {
        m_isChosen = ic;
    }

    /**
     * @brief getter for element
     *
     * @return
     */
    T getElem() const {
        return m_elem;
    }

    /**
     * @brief operator== tricky!!!
     *
     * @param se
     *
     * @return
     */
    bool operator==(const Facility & se) const {
        //TODO this is to tricky
        return /*m_isChosen == se.m_isChosen && */m_elem == se.m_elem;
    }

    /**
     * @brief operator<<
     *
     * @tparam ostream
     * @tparam TT
     * @param o
     * @param f
     *
     * @return
     */
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
