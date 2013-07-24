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
    
    template <typename Names, typename Types>
    class Components;
    
    template <>
    class Components<TypesVector<>, TypesVector<>> {
    public:
        void get() const {}
        
        Components() = default;

        template <typename Comps>
        Components(const Comps & comps) {} 
    };

    template <typename Name, typename Type, typename... NamesRest, typename... TypesRest>
    class Components<TypesVector<Name, NamesRest...>, TypesVector<Type, TypesRest...>> { 
        typedef Components<TypesVector<NamesRest...>, TypesVector<TypesRest...>> base;
        typedef TypesVector<Name, NamesRest...> Names;
        typedef TypesVector<Type, TypesRest...> Types;

    public:
        Components() = default;
        
        Components(const Components &) = default;
        
        Components(Components &&) = default;

        template <typename T, typename... TypesPrefix>
        Components(T t, TypesPrefix... types) : 
            m_base(std::move(types)...), m_component(std::move(t)) 
        {}
       
        //constructor takes Components class with appropriate signature
        template <typename Types2>
        Components(Components<Names, Types2> comps) : 
            m_base(std::move(static_cast<Components<TypesVector<NamesRest...>,
                                typename remove_n_first<1, Types2>::type>                           
                            >(comps))), 
            m_component(std::move(comps.template get<Name>())) {} 
        
        //constructor takes Components class with appropriate signature
        template <typename Comps, typename dummy = typename std::enable_if<HasTemplateGet<Comps, Name>::value>::type  >
        Components(Comps comps) : 
            m_base(comps), 
            m_component(std::move(comps.template get<Name>())) {} 

        template <typename ComponentName, typename... Args>
        auto call(Args&&... args) ->
        decltype(std::declval<typename TypeForName<ComponentName, Names, Types>::type>()(std::forward<Args>(args)...)) {
            return get<ComponentName>()(std::forward<Args>(args)...);
        }

        template <typename ComponentName>
        void set(const typename TypeForName<ComponentName, Names, Types>::type  comp) {
            get<ComponentName>() = std::move(comp);
        }
        
        
        template <typename ComponentName>
        typename TypeForName<ComponentName, Names, Types>::type & get() {
            return get(WrapToConstructable<ComponentName>());
        }
        
        template <typename ComponentName>
        const typename TypeForName<ComponentName, Names, Types>::type & get() const {
            return get(WrapToConstructable<ComponentName>());
        }
        
    protected:
        template <typename ComponentName>
        const typename TypeForName<ComponentName, Names, Types>::type & 
        get(WrapToConstructable<ComponentName> w) const {
            return m_base.template get<ComponentName>();
        }
        
        const Type & get(WrapToConstructable<Name>) const {
            return m_component;
        }
        
        template <typename ComponentName>
        typename TypeForName<ComponentName, Names, Types>::type & 
        get(WrapToConstructable<ComponentName> w) { 
            return m_base.template get<ComponentName>();
        }
        
        Type & get(WrapToConstructable<Name>) {
            return m_component;
        }
        
        //we cannot use inheritance (even private) because gcc 4.7.3 says  " is an ambiguous base class of..." !
        base m_base;
        Type m_component;
    };
} // detail



/**
 * @brief This namespace contains class which sets all defaults and all needed meta functions.
 */
namespace detail_set_defaults {
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
    
    template <typename NamesWithDefaults, typename TypesPrefix>
    class SetDefaults {
        static const int N = size<NamesWithDefaults>::value;
        static const int TYPES_NR = size<TypesPrefix>::value;
        static_assert(TYPES_NR <= N, "Incrrect number of parameters");


        typedef typename fold<
                NamesWithDefaults,
                TypesVector<>,
                PushBackName
            >::type Names;

        typedef typename fold<
                NamesWithDefaults,
                TypesVector<>,
                PushBackDefault
            >::type Defaults;
        
        static const int DEFAULTS_NR = size<Defaults>::value;
        static_assert(DEFAULTS_NR + TYPES_NR >= N, "Incrrect number of parameters");

        typedef typename remove_n_first<DEFAULTS_NR + TYPES_NR - N, Defaults>::type NeededDefaults;

        typedef typename join<TypesPrefix, NeededDefaults>::type Types;
    public:
        typedef detail::Components<Names, Types> type;
    private:
        //in this block we check if the defaults are on the last positions in the NamesWithDefaults
    };
} //detail_set_defaults


//direct implementation on variadic templates is imposible because of
//weak support for type detection for inner template classes
template <typename... ComponentNamesWithDefaults>
class Components {
    typedef TypesVector<ComponentNamesWithDefaults...> NamesWithDefaults;
public:
    template <typename... ComponentTypes>
    using type = typename detail_set_defaults::SetDefaults<NamesWithDefaults, TypesVector<ComponentTypes...>>::type;

    template <typename... Components>
    static 
    type<Components...>
    make_components(Components... comps) {
        return type<Components...>(std::move(comps)...);
    }
};


} //data_structures
}//paal
#endif /* COMPONENTS_HPP */
