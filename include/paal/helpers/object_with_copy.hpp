/**
 * @file object_with_copy.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-02-01
 */
template <typename T> class ObjectWithCopy {
public:
    typedef T ObjectType;

    ObjectWithCopy(T t) : m_obj(std::move(t)), m_copy(m_obj) {}

    // if you use *. in decltype instead of -> you get
    // "sorry, unimplemented: mangling dotstar_expr" :)
    template <typename F, typename... Args> 
        auto invoke(F f, Args... args) 
            -> decltype(((std::declval<T*>())->*(f))(args...)) {
        (m_copy.*(f))(args...);
        return (m_obj.*(f))(args...);
    }
    
    template <typename F, typename... Args> 
        auto invokeOnCopy(F f, Args... args) const
            -> decltype(((std::declval<T*>())->*(f))(args...)) {
        return (m_copy.*(f))(args...);
    }

    const T * operator->() const {
        return &m_obj;
    }

    T & getObj() {
        return m_obj;
    }

private:
    T m_obj;
    mutable T m_copy;

};

