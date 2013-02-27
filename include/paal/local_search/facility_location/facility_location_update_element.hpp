/**
 * @file facility_location_update_element.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */
#ifndef  FACILITY_LOCATION_UPDATE_ELEMENT_HPP
#define FACILITY_LOCATION_UPDATE_ELEMENT_HPP

namespace paal {
namespace local_search {
namespace facility_location {

enum UpdateType {REMOVE, ADD, SWAP};

class AbstractUpdate {
protected:
    virtual ~AbstractUpdate() {}
public:
    virtual UpdateType getType() const = 0;
};

template <typename T> class Swap : public AbstractUpdate {
public:
    Swap(T from, T to) : m_from(from), m_to(to) {}
    Swap() {}

    UpdateType getType() const {
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


template <typename T> class RemoveAndAddBase : public AbstractUpdate {
public:
    RemoveAndAddBase(T t) : m_t(t) {}
    RemoveAndAddBase() {}

    T get() const {
        return m_t;
    }
    
    void set(T t) {
        m_t = t;
    }

private:
    T m_t;
};

template <typename T> struct Remove : public RemoveAndAddBase<T> {
    Remove(T t) : RemoveAndAddBase<T>(t) {}
    Remove() {}
    UpdateType getType() const {
        return REMOVE;
    }
};

template <typename T> struct Add : public RemoveAndAddBase<T> {
    Add(T t) : RemoveAndAddBase<T>(t) {}
    Add() {}
    UpdateType getType() const {
        return ADD;
    }
};

class Update {
public:
    template <typename T> Update(T * t) : m_impl(t) {}

    const AbstractUpdate * getImpl() const {
        return m_impl;
    }

private:
    AbstractUpdate * m_impl;
};


} // facility_location
} // local_search
} //paal


#endif // FACILITY_LOCATION_UPDATE_ELEMENT_HPP
