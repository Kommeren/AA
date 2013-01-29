#ifndef __FC_SOLUTION_ELEMENT__
#define __FC_SOLUTION_ELEMENT__


namespace paal {
namespace local_search {
namespace facility_location {

enum is_chosen { CHOSEN, UNCHOSEN};

template <typename T> class SolutionElement {
public:
    SolutionElement(is_chosen ic, T e) : m_isChosen(ic), m_elem(e) {}

    is_chosen getIsChosen() const {
        return m_isChosen;
    }

    T getElem() const {
        return m_elem;
    }

    bool operator==(const SolutionElement & se) const {
        return m_isChosen == se.m_isChosen && m_elem == se.m_elem;
    }

private:
    is_chosen m_isChosen;
    T m_elem;    
};

template <typename stream, typename T> stream & operator<<(stream & o, const SolutionElement<T> & se) {
    o << se.getIsChosen() << "," << se.getElem() << " ";
    return o;
}


}
}
}


#endif //__FC_SOLUTION_ELEMENT__
