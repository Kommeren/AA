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

struct FacilityHash {
    template <typename T>
    size_t operator()(Facility<T> f) const {
        return std::hash<T>()(f.getElem());
    }
};


}
}
}


#endif //FACILITY_LOCATION_SOLUTION_ELEMENT_HPP
