/**
 * @file components.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-07-16
 */
#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP 

#include <utility>
#include <iostream>

#include "types_vector.hpp"


namespace paal {
namespace data_structures {

/**
 * @brief This structure can be passed on Names list and represents Name and the default type value
 *
 * @tparam Name
 * @tparam Default
 */
template <typename Name, typename Default>
struct NameWithDefault;


/**
 * @brief Indicates that Components constructor is in fact a Copy/Move Constructor
 */
struct CopyTag {};


/**
 * @brief This namespace block contains implementation of the main class Components<Names,Types> and needed meta functions
 */
namespace detail {

    /**
     * @brief wraps type to constructible type
     *
     * @tparam T
     */
    template <typename T>
    struct WrapToConstructable {
        typedef T type;
    };

    /**
     * @brief If Name is kth on Names list, returns kth Type.
     *
     * @tparam Name
     * @tparam Names
     * @tparam Types
     */
    template <typename Name, typename Names, typename Types>
    struct TypeForName {
        typedef typename remove_n_first<1, Names>::type NewNames;
        typedef typename remove_n_first<1, Types>::type NewTypes;
        typedef typename TypeForName<Name, NewNames, NewTypes>::type type;
    };

    /**
     * @brief Specialization when found
     *
     * @tparam Name
     * @tparam Type
     * @tparam NamesRest
     * @tparam TypesRest
     */
    template <typename Name,typename Type, typename... NamesRest, typename... TypesRest>
    struct TypeForName<Name, TypesVector<Name, NamesRest...>, TypesVector<Type, TypesRest...>>{
        typedef Type type;
    };


    /**
     * @brief SFINAE check if the given type has get<Name>() member function.
     *
     * @tparam T
     * @tparam Name
     */
    template <typename T, typename Name> 
    class HasTemplateGet {  
    private: 
        /**
         * @brief positive case
         *
         * @tparam C given type
         *
         * @return return type is char 
         */
        template <typename C>
            static char f(WrapToConstructable<decltype(std::declval<const C>().template get<Name>()) (C::*)() const>*);

        /**
         * @brief negative case
         *
         * @tparam C given type
         *
         * @return return type is long
         */
        template <typename C>
            static long f(...); 

    public:
        /**
         * @brief tels if given type has get<Name>() memer function. 
         *
         */
        static  const bool value = (sizeof(f<typename std::decay<T>::type>(nullptr)) == sizeof(char));
    }; 



    /**
     * @brief Tag indicating that given object is movable
     */
    struct MovableTag{};
    /**
     * @brief Tag indicating that given object is not movable
     */
    struct NotMovableTag{};


    //declaration of main class Components
    template <typename Names, typename Types>
    class Components;

    //specialization for empty Names list
    template <>
    class Components<TypesVector<>, TypesVector<>> {
    public:
        void get() const;
        void call() const;
        void call2() const;

        template <typename... Unused>
        Components(const Unused &... ) {} 
    };

    //specialization for nonempty types list
    //class keeps first component as data memer
    //rest of the components are kept in superclass.
    template <typename Name, typename Type, typename... NamesRest, typename... TypesRest>
    class Components<TypesVector<Name, NamesRest...>, TypesVector<Type, TypesRest...>> : 
    public Components<TypesVector<NamesRest...>, TypesVector<TypesRest...>> { 
        typedef Components<TypesVector<NamesRest...>, TypesVector<TypesRest...>> base;
        typedef TypesVector<Name, NamesRest...> Names;
        typedef TypesVector<Type, TypesRest...> Types;
        
        
        /**
         * @brief Evaluates to valid type iff ComponentsName == Name
         *
         * @tparam ComponentName
         */
        template <typename ComponentName>
        using is_my_name = typename std::enable_if<std::is_same<ComponentName, Name>::value>::type;

    public:
        using base::get;

        //default constructor
        Components() = default;

        //default copy constructor
        Components(const Components &) = default;
        
        //doesn't work on clang 3.2 // change in the standard
        //Components(Components &) = default;
        //constructor taking nonconst lvalue reference
        Components(Components & comps) : 
            base(static_cast<base &>(comps)), 
            m_component(comps.get<Name>()) {}
        
        //default move constructor
        Components(Components &&) = default;

        //default assignemnt operator
        Components& operator=(const Components &) = default;

        //doesn't work on clang 3.2 // change in the standard
        //Components& operator=(Components &) = default;
        //assignemnt operator taking nonconst lvalue reference
        Components& operator=(Components & comps) {
            static_cast<base &>(*this) = static_cast<base &>(comps); 
            m_component = comps.get<Name>();
            return *this;
        }

        //default move operator
        Components& operator=(Components &&) = default;

        
        /**
         * @brief constructor takes some number of arguments,
         *      This arguments has to be convertible to the same number of the first components in Components class.
         *      Arguments can be both rvalue and lvalue references
         *
         * @tparam T, first component, it must be convertible to Type.
         * @tparam TypesPrefix, rest of the components
         * @param t
         * @param types
         */
        template <typename T, typename... TypesPrefix>
        Components(T&& t, TypesPrefix&&... types) : 
            base(std::forward<TypesPrefix>(types)...), m_component(std::forward<T>(t)) 
        {}

        //     copy constructor takes class wich has get<Name> member function
        //     the get<> function dosn't have to be available for all names.
        //     @param CopyTag is helps identify  this constructor
        template <typename Comps>
        Components(const Comps & comps, CopyTag) : 
            Components(comps, NotMovableTag()) {}

        //   move  constructor takes class wich has get<Name> member function
        //     the get<> function dosn't have to be available for all names.
        //     In this version each of the components taken from comps 
        //     is going to be moved.
        //     @param CopyTag is helps identify  this constructor
        template <typename Comps>
        Components(Comps&& comps, CopyTag) : 
                Components(comps, MovableTag()) {}

        /**
         * @brief This fucntion returns Component for name Name, nonconst version
         *
         * @tparam ComponentName
         * @tparam typename
         * @param dummy
         *
         * @return 
         */
        template <typename ComponentName, typename = is_my_name<ComponentName>>
        Type & get(WrapToConstructable<Name> dummy = WrapToConstructable<Name>()) {
            return m_component;
        }

        /**
         * @brief This fucntion returns Component for name Name, const version
         *
         * @tparam ComponentName
         * @tparam typename
         * @param dummy
         *
         * @return 
         */
        template <typename ComponentName, typename = is_my_name<ComponentName>>
        const Type & get(WrapToConstructable<Name> dummy = WrapToConstructable<Name>()) const{
            return m_component;
        }

        /**
         * @brief This function directly calls component.
         *        m_component(args) has to be valid expresion
         *        nonconst version
         *
         * @tparam ComponentName
         * @tparam Args
         * @param args call arguments 
         *
         * @return 
         */
        template <typename ComponentName, typename... Args>
        auto call(Args&&... args) ->
        decltype(std::declval<typename TypeForName<ComponentName, Names, Types>::type>()(std::forward<Args>(args)...)) {
            return this->template get<ComponentName>()(std::forward<Args>(args)...);
        }

        /**
         * @brief This function directly calls component.
         *        m_component(args) has to be valid expresion
         *        const version
         *
         * @tparam ComponentName
         *
         * @tparam ComponentName
         * @tparam Args
         * @param call arguments
         *
         * @return 
         */
        template <typename ComponentName, typename... Args>
        auto call(Args&&... args) const ->
        decltype(std::declval<const typename TypeForName<ComponentName, Names, Types>::type>()(std::forward<Args>(args)...)) {
            return this->template get<ComponentName>()(std::forward<Args>(args)...);
        }

        /**
         * @brief setter for component assigned to Name.
         *
         * @tparam ComponentName
         * @param comp
         */
        template <typename ComponentName>
        void set(const typename TypeForName<ComponentName, Names, Types>::type  comp) {
            get<ComponentName>() = std::move(comp);
        }


        /**
         * @brief function creating Components class,
         *        takes arguments only for assigned Names
         *
         * @tparam NamesSubset
         * @tparam SomeTypes
         * @param types
         *
         * @return 
         */
        template <typename... NamesSubset, typename... SomeTypes>
        static Components<Names, Types>
        //make(SomeTypes... types) {
          //  static_assert(sizeof...(NamesSubset) == sizeof...(SomeTypes), "Incorrect number of arguments.");
            //return Components<Names, Types>(Components<TypesVector<NamesSubset...>, TypesVector<SomeTypes...>>(std::move(types)...), CopyTag());
        make(SomeTypes&&... types) {
            static_assert(sizeof...(NamesSubset) == sizeof...(SomeTypes), "Incorrect number of arguments.");
            Components<TypesVector<NamesSubset...>, TypesVector<SomeTypes...>> comps(std::forward<SomeTypes>(types)...);
            return Components<Names, Types>(std::move(comps), CopyTag());
        }


    protected:
        //All of this constructor takes Comps as r-value reference, 
        //because they have to win specialization race with normal constructor.

        //case: movable object, has the appropriate get member function, Type is not reference
        template <typename Comps, 
                  typename dummy =     typename std::enable_if<HasTemplateGet<Comps, Name    >::value, int>::type,
                  typename dummy_ref = typename std::enable_if<!std::is_lvalue_reference<Type>::value>::type>
        Components(Comps && comps, MovableTag m, dummy d = dummy()) : 
            base(std::forward<Comps>(comps), std::move(m)), 
            m_component(std::move(comps.template get<Name>())) {} 
        
        //case: movable object, has the appropriate get member function, Type is reference
        template <typename Comps, 
                  typename dummy =     typename std::enable_if<HasTemplateGet<Comps, Name   >::value, int>::type,
                  typename dummy_ref = typename std::enable_if<std::is_lvalue_reference<Type>::value, int>::type>
        Components(Comps && comps, MovableTag m, dummy d = dummy(), dummy_ref dr = dummy_ref()) : 
            base(std::forward<Comps>(comps), std::move(m)), 
            m_component(comps.template get<Name>()) {} 

        //case: movable object, does not have the appropriate get member function
        template <typename Comps, typename dummy = typename std::enable_if<!HasTemplateGet<Comps, Name>::value>::type>
        Components(Comps && comps, MovableTag m) : 
            base(std::forward<Comps>(comps), std::move(m)) {} 

        //case: not movable object, has the appropriate get member function
        template <typename Comps, typename dummy = typename std::enable_if<HasTemplateGet<Comps, Name>::value, int>::type>
        Components(Comps && comps, NotMovableTag m, dummy d = dummy()) : 
            base(std::forward<Comps>(comps), std::move(m)), 
            m_component(comps.template get<Name>()) {} 

        //case: not movable object, does not  have the appropriate get member function
        template <typename Comps, typename dummy = typename std::enable_if<!HasTemplateGet<Comps, Name>::value>::type>
        Components(Comps && comps, NotMovableTag m) : 
            base(std::forward<Comps>(comps), std::move(m)) {} 
    private:
        Type m_component;
    };
} // detail



/**
 * @brief This namespace contains class which sets all defaults and all needed meta functions.
 */
namespace detail {

    template <typename Names, typename Defaults, typename TypesPrefix>
    class SetDefaults {
        static const int N = size<Names>::value;
        static const int TYPES_NR = size<TypesPrefix>::value;
        static_assert(TYPES_NR <= N, "Incrrect number of parameters");

        static const int DEFAULTS_NR = size<Defaults>::value;
        static_assert(DEFAULTS_NR + TYPES_NR >= N, "Incrrect number of parameters");

        typedef typename remove_n_first<DEFAULTS_NR + TYPES_NR - N, Defaults>::type NeededDefaults;

        typedef typename join<TypesPrefix, NeededDefaults>::type Types;
    public:
        typedef detail::Components<Names, Types> type;
    };
} //detail



/**
 * @brief Here are some meta functions, to parse the arguments 
 */
namespace detail {
    /**
     * @brief GetName, gets name for either Name, or NamesWithDefaults struct
     *        this is the Name case
     *
     * @tparam T
     */
    template <typename T>
    struct GetName {
        typedef T type;
    };

    /**
     * @brief GetName, gets name for either Name, or NamesWithDefaults struct
     *        this is the NamesWithDefaults case
     *
     * @tparam Name 
     * @tparam Default
     */
    template <typename Name, typename Default>
    struct GetName<NameWithDefault<Name, Default>> {
        typedef Name type;
    };

    /**
     * @brief Meta function takes NameWithDefault and Vector
     *        the result is new vector with new Name appended Name
     */
    struct PushBackName {
    template <typename Vector, typename NameWithDefault>
        struct apply {
            typedef typename push_back<Vector, typename GetName<NameWithDefault>::type>::type type;
        };
    };

    /*
     * @brief Meta function takes NameWithDefault and Vector
     *        the result is new vector with new Name appended Default
     */
    struct PushBackDefault {
        //  This case applies to when NameWithDefault is only name
        template <typename Vector, typename Name>
        struct apply {
            typedef Vector type;
        };
   
        // This case applies when NameWithDefault contains Default
        template <typename Vector, typename Name, typename Default>
        struct apply<Vector, NameWithDefault<Name, Default>> {
            typedef typename push_back<Vector, Default>::type type;
        };

    };
} //detail


//this is class sets all defaults and return as type detail::Components<Names, Types>
//direct implementation on variadic templates is imposible because of
//weak support for type detection for inner template classes
template <typename... ComponentNamesWithDefaults>
class Components {
    typedef TypesVector<ComponentNamesWithDefaults...> NamesWithDefaults;

    //get Names list from NamesWithDefaults
    typedef typename fold<
        NamesWithDefaults,
        TypesVector<>,
        detail::PushBackName
            >::type Names;

    //get Defaults from NamesWithDefaults
    typedef typename fold<
        NamesWithDefaults,
        TypesVector<>,
        detail::PushBackDefault
            >::type Defaults;

    template <class T>
    struct special_decay {
        using type = typename std::decay<T>::type;
    };
     
    template <class T>
    struct special_decay<std::reference_wrapper<T>> {
        using type = T&;
    };

    template <class T>
         using special_decay_t = typename special_decay<T>::type;

public:
    template <typename... ComponentTypes>
    using type = typename detail::SetDefaults<Names, Defaults, TypesVector<ComponentTypes...>>::type;


    template <typename... Components>
    static 
    type<special_decay_t<Components>...>
    make_components(Components&&... comps) {
        return type<special_decay_t<Components>...>(std::forward<Components>(comps)...);
    }
private:
    //in this block we check if the defaults are on the last positions in the NamesWithDefaults
    static const int N = size<NamesWithDefaults>::value;
    static const int DEFAULTS_NR = size<Defaults>::value;
    typedef typename remove_n_first<N - DEFAULTS_NR, NamesWithDefaults>::type DefaultPart;
    typedef typename fold<
        DefaultPart,
        TypesVector<>,
        detail::PushBackDefault
            >::type DefaultsTest;
    static_assert(std::is_same<DefaultsTest, Defaults>::value, "Defaults values could be only on subsequent number of last parameters");
};


} //data_structures
}//paal
#endif /* COMPONENTS_HPP */
