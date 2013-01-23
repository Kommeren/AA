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

private:
    is_chosen m_isChosen;
    T m_elem;    
};

