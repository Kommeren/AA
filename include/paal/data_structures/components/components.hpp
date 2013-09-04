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


struct CopyTag {};


/**
 * @brief This namespace block contains implementation of the main class Components<Names,Types> and needed meta functions
 */
namespace detail {
    template <typename T>
    struct WrapToConstructable {
        typedef T type;
    };

    template <typename Name, typename Names, typename Types>
    struct TypeForName {
        typedef typename remove_n_first<1, Names>::type NewNames;
        typedef typename remove_n_first<1, Types>::type NewTypes;
        typedef typename TypeForName<Name, NewNames, NewTypes>::type type;
    };

    template <typename Name,typename Type, typename... NamesRest, typename... TypesRest>
    struct TypeForName<Name, TypesVector<Name, NamesRest...>, TypesVector<Type, TypesRest...>>{
        typedef Type type;
    };

    template <typename T, typename Name> 
    class HasTemplateGet {  
    private: 
        template <typename U> 
            class check 
            { }; 

        template <typename C>
            static char f(check<decltype(std::declval<const C>().template get<Name>()) (C::*)() const>*);

        template <typename C>
            static long f(...); 

    public:
        static  const bool value = (sizeof(f<typename std::decay<T>::type>(nullptr)) == sizeof(char));
    }; 

    struct Movable;
    struct NotMovable;

    template <typename Names, typename Types>
    class Components;

    template <>
    class Components<TypesVector<>, TypesVector<>> {
    public:
        void get() const;

        template <typename... Unused>
        Components(const Unused &... ) {} 
    };

    template <typename Name, typename Type, typename... NamesRest, typename... TypesRest>
    class Components<TypesVector<Name, NamesRest...>, TypesVector<Type, TypesRest...>> : 
    public Components<TypesVector<NamesRest...>, TypesVector<TypesRest...>> { 
        typedef Components<TypesVector<NamesRest...>, TypesVector<TypesRest...>> base;
        typedef TypesVector<Name, NamesRest...> Names;
        typedef TypesVector<Type, TypesRest...> Types;

    public:
        using base::get;

        Components() = default;

        Components(const Components &) = default;
        
        //doesn't work on clang 3.2 // change in the standard
        //Components(Components &) = default;
        Components(Components & comps) : 
            base(static_cast<base &>(comps)), 
            m_component(comps.get<Name>()) {}
        
        Components(Components &&) = default;

        Components& operator=(const Components &) = default;

        //doesn't work on clang 3.2 // change in the standard
        //Components& operator=(Components &) = default;
        Components& operator=(Components & comps) {
            static_cast<base &>(*this) = static_cast<base &>(comps); 
            m_component = comps.get<Name>();
            return *this;
        }

        Components& operator=(Components &&) = default;

        template <typename T, typename... TypesPrefix>
        Components(T&& t, TypesPrefix&&... types) : 
            base(std::forward<TypesPrefix>(types)...), m_component(std::forward<T>(t)) 
        {}

        //     copy constructor takes class wich has get<Name> member function
        template <typename Comps>
        Components(const Comps & comps, CopyTag) : 
            Components(comps, WrapToConstructable<NotMovable>()) {}

        //   move  constructor takes class wich has get<Name> member function
        template <typename Comps>
        Components(Comps&& comps, CopyTag) : 
                Components(comps, WrapToConstructable<Movable>()) {}

        template <typename ComponentName, typename = typename std::enable_if<std::is_same<ComponentName, Name>::value>::type>
        Type & get(WrapToConstructable<Name> dummy = WrapToConstructable<Name>()) {
            return m_component;
        }

        template <typename ComponentName, typename = typename std::enable_if<std::is_same<ComponentName, Name>::value>::type>
        const Type & get(WrapToConstructable<Name> dummy = WrapToConstructable<Name>()) const{
            return m_component;
        }

        template <typename ComponentName, typename... Args>
        auto call(Args&&... args) ->
        decltype(std::declval<typename TypeForName<ComponentName, Names, Types>::type>()(std::forward<Args>(args)...)) {
            return this->template get<ComponentName>()(std::forward<Args>(args)...);
        }

        template <typename ComponentName, typename... Args>
        auto call(Args&&... args) const ->
        decltype(std::declval<const typename TypeForName<ComponentName, Names, Types>::type>()(std::forward<Args>(args)...)) {
            return get<ComponentName>()(std::forward<Args>(args)...);
        }

        template <typename ComponentName>
        void set(const typename TypeForName<ComponentName, Names, Types>::type  comp) {
            get<ComponentName>() = std::move(comp);
        }


        template <typename... NamesSubset, typename... SomeTypes>
        static Components<Names, Types>
        make(SomeTypes... types) {
            static_assert(sizeof...(NamesSubset) == sizeof...(SomeTypes), "Incorrect number of arguments.");
            return Components<Names, Types>(Components<TypesVector<NamesSubset...>, TypesVector<SomeTypes...>>(std::move(types)...), CopyTag());
        }


    protected:
        //All of this constructor takes Comps as r-value reference, 
        //because they have to win specialization race with normal constructor.

        //case: movable object, has the appropriate get member function
        template <typename Comps, typename dummy = typename std::enable_if<HasTemplateGet<Comps, Name>::value, int>::type>
        Components(Comps && comps, WrapToConstructable<Movable> m, dummy d = dummy()) : 
            base(std::forward<Comps>(comps), std::move(m)), 
            m_component(std::move(comps.template get<Name>())) {} 

        //case: movable object, does not have the appropriate get member function
        template <typename Comps, typename dummy = typename std::enable_if<!HasTemplateGet<Comps, Name>::value>::type>
        Components(Comps && comps, WrapToConstructable<Movable> m) : 
            base(std::forward<Comps>(comps), std::move(m)) {} 

        //case: not movable object, has the appropriate get member function
        template <typename Comps, typename dummy = typename std::enable_if<HasTemplateGet<Comps, Name>::value, int>::type>
        Components(Comps && comps, WrapToConstructable<NotMovable> m, dummy d = dummy()) : 
            base(std::forward<Comps>(comps), std::move(m)), 
            m_component(comps.template get<Name>()) {} 

        //case: not movable object, does not  have the appropriate get member function
        template <typename Comps, typename dummy = typename std::enable_if<!HasTemplateGet<Comps, Name>::value>::type>
        Components(Comps && comps, WrapToConstructable<NotMovable> m) : 
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

    struct PushBackName {
    template <typename Vector, typename NameWithDefault>
        struct apply {
            typedef typename push_back<Vector, typename GetName<NameWithDefault>::type>::type type;
        };
    };

    struct PushBackDefault {
    template <typename Vector, typename Name>
    struct apply {
        typedef Vector type;
    };

    template <typename Vector, typename Name, typename Default>
    struct apply<Vector, NameWithDefault<Name, Default>> {
        typedef typename push_back<Vector, Default>::type type;
    };

    };
} //detail


//direct implementation on variadic templates is imposible because of
//weak support for type detection for inner template classes
template <typename... ComponentNamesWithDefaults>
class Components {
    typedef TypesVector<ComponentNamesWithDefaults...> NamesWithDefaults;
    typedef typename fold<
        NamesWithDefaults,
        TypesVector<>,
        detail::PushBackName
            >::type Names;

    typedef typename fold<
        NamesWithDefaults,
        TypesVector<>,
        detail::PushBackDefault
            >::type Defaults;
public:
    template <typename... ComponentTypes>
    using type = typename detail::SetDefaults<Names, Defaults, TypesVector<ComponentTypes...>>::type;

    template <typename... Components>
    static 
    type<Components...>
    make_components(Components... comps) {
        return type<Components...>(std::move(comps)...);
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
