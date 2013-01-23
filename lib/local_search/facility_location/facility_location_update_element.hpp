
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

    UpdateType getType() {
        return SWAP;
    }

    T getFrom() const {
        return m_from;
    }

    T getTo() const {
        return m_to;
    }
private:
    T m_from;
    T m_to;
};


template <typename T> class RemAdd : public AbstractUpdate {
public:
    RemAdd(T t) : m_t(t) {}

    T get() const {
        return m_t;
    }
private:
    T m_t;
};

template <typename T> struct Remove : public RemAdd<T> {
    Remove(T t) : RemAdd<T>(t) {}
    UpdateType getType() {
        return REMOVE;
    }
};

template <typename T> struct Add : public RemAdd<T> {
    Add(T t) : RemAdd<T>(t) {}
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
