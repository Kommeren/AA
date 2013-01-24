#ifndef  __FC_UPDATE_ELEMENT__
#define __FC_UPDATE_ELEMENT__

namespace paal {
namespace local_search {
namespace facility_location {

enum UpdateType {REMOVE, ADD, SWAP};

class AbstractUpdate {
protected:
    virtual ~AbstractUpdate() {}
public:
    virtual UpdateType getType() = 0;
};

template <typename T> class Swap : public AbstractUpdate {
public:
    Swap(T from, T to) : m_from(from), m_to(to) {}
    Swap() {}

    UpdateType getType() {
        return SWAP;
    }

    T getFrom() const {
        return m_from;
    }

    T getTo() const {
        return m_to;
    }
    
    void setFrom(T from) {
        m_from = from;
    }

    void setTo(T to) {
        m_to = to;
    }

private:
    T m_from;
    T m_to;
};


template <typename T> class RemAdd : public AbstractUpdate {
public:
    RemAdd(T t) : m_t(t) {}
    RemAdd() {}

    T get() const {
        return m_t;
    }
    
    void set(T t) {
        m_t = t;
    }

private:
    T m_t;
};

template <typename T> struct Remove : public RemAdd<T> {
    Remove(T t) : RemAdd<T>(t) {}
    Remove() {}
    UpdateType getType() {
        return REMOVE;
    }
};

template <typename T> struct Add : public RemAdd<T> {
    Add(T t) : RemAdd<T>(t) {}
    Add() {}
    UpdateType getType() {
        return ADD;
    }
};

class UpdateElement {
public:
    template <typename T> UpdateElement(T * t) : m_impl(t) {}

    AbstractUpdate * getImpl() {
        return m_impl;
    }

private:
    AbstractUpdate * m_impl;
};


} // facility_location
} // local_search
} //paal


#endif // __FC_UPDATE_ELEMENT__
