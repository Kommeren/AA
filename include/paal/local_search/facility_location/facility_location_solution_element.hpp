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

template <typename T> class Facility {
public:
    Facility(is_chosen ic, T e) : m_isChosen(ic), m_elem(e) {}

    is_chosen getIsChosen() const {
        return m_isChosen;
    }

    T getElem() const {
        return m_elem;
    }

    bool operator==(const Facility & se) const {
        return m_isChosen == se.m_isChosen && m_elem == se.m_elem;
    }

private:
    is_chosen m_isChosen;
    T m_elem;    
};

template <typename stream, typename T> stream & operator<<(stream & o, const Facility<T> & se) {
    o << se.getIsChosen() << "," << se.getElem() << " ";
    return o;
}


}
}
}


#endif //FACILITY_LOCATION_SOLUTION_ELEMENT_HPP
