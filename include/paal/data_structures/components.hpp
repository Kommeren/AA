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

#include <boost/mpl/if.hpp>

#include "types_vector.hpp"


namespace paal {
namespace data_structures {



template <typename Name, typename Default>
struct NameWithDefault;

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

    template <typename Name,typename Type, typename... Args1, typename... Args2>
    struct TypeForName<Name, TypesVector<Name, Args1...>, TypesVector<Type, Args2...>>{
        typedef Type type;
    };

    template <typename T, typename Name> 
    class HasTemplateGet {  
        private: 
            template <typename U, U> 
                class check 
                { }; 

            template <typename C>
                static char f(check<decltype(std::declval<const C>().template get<Name>()) (C::*)() const, &C::template get<Name>>*);

            template <typename C>
                static long f(...); 

        public:
            static  const bool value = (sizeof(f<typename std::decay<T>::type>(0)) == sizeof(char));
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
    class Components<TypesVector<Name, NamesRest...>, TypesVector<Type, TypesRest...>> : 
            Components<TypesVector<NamesRest...>, TypesVector<TypesRest...>> {
        typedef Components<TypesVector<NamesRest...>, TypesVector<TypesRest...>> base;
        typedef TypesVector<Name, NamesRest...> Names;
        typedef TypesVector<Type, TypesRest...> Types; 
    public:
        Components() = default;

        template <typename T, typename... TypesPrefix>
        Components(T t, TypesPrefix ... types) : base(types...), m_component(std::move(t)) 
        {}
       
        //constructor takes Components class with appropriate signature
        template <typename Types2>
        Components(Components<Names, Types2> comps) : 
            base(std::move(static_cast<Components<TypesVector<NamesRest...>,
                                typename remove_n_first<1, Types2>::type>                           
                            >(comps))), 
            m_component(std::move(comps.template get<Name>())) {} 
        
        //constructor takes Components class with appropriate signature
        template <typename Comps, typename dummy = typename std::enable_if<HasTemplateGet<Comps, Name>::value>::type  >
        Components(Comps comps) : 
            base(comps), 
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
            return base::get(w);
        }
        
        const Type & get(WrapToConstructable<Name>) const {
            return m_component;
        }
        
        template <typename ComponentName>
        typename TypeForName<ComponentName, Names, Types>::type & 
        get(WrapToConstructable<ComponentName> w) { 
            return base::get(w);
        }
        
        Type & get(WrapToConstructable<Name>) {
            return m_component;
        }

        Type m_component;
    };
    
    template <typename T>
    struct GetName {
        typedef T type;
    };

    template <typename Name, typename Default>
    struct GetName<NameWithDefault<Name, Default>> {
        typedef Name type;
    };

    struct NoDefault;

    template <typename T>
    struct GetDefault {
        typedef NoDefault type;
    };

    template <typename Name, typename Default>
    struct GetDefault<NameWithDefault<Name, Default>> {
        typedef Default type;
    };

    struct PushBackName {
        template <typename Vector, typename NameWithDefault>
        struct apply {
            typedef typename push_back<Vector, typename GetName<NameWithDefault>::type>::type type;
        };
    };
    
    struct PushBackDefault {
        template <typename Vector, typename NameWithDefault>
        struct apply {
            typedef typename GetDefault<NameWithDefault>::type Default;

            typedef typename 
                boost::mpl::if_<std::is_same<Default, NoDefault>,
                            Vector, //then
                            typename push_back<Vector, Default>::type //else
                        >::type  type;
        };
    };
    
    template <typename NamesWithDefaults, typename TypesPrefix>
    class SetDefaults {
    public:
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

        public:
        typedef typename join<TypesPrefix, NeededDefaults>::type Types;
        typedef Components<Names, Types> type;
    };

};


//direct implementation on variadic templates is imposible because of
//week support for type detection for inner template classes
template <typename... ComponentNamesWithDefaults>
class Components {
    typedef TypesVector<ComponentNamesWithDefaults...> NamesWithDefaults;
public:
    template <typename... ComponentTypes>
    using type = typename detail::SetDefaults<NamesWithDefaults, TypesVector<ComponentTypes...>>::type;
};


template <typename Name, typename NewType, typename Components> class SwapType;

template <typename Name, typename NewType, typename Names, typename Types> 
class SwapType<Name, NewType, detail::Components<Names, Types>> {
    static const int p =  pos<Name, Names>::value; // position to insert
    typedef typename replace_at_pos<p, NewType, Types>::type TypesSwapped;
public:
    typedef detail::Components<Names, TypesSwapped> type;
};

namespace detail {
    template <typename Comp>
    struct get_types;
    
    template <typename Names, typename Types>
    struct get_types<Components<Names, Types>> {
        typedef Types type;
    };
    

    template <typename Name, typename NewType, typename Names, typename Types> 
    class TempSwapped {
        typedef detail::Components<Names, Types> Comps;
        typedef typename SwapType<Name, NewType, Comps>::type Swapped;
        typedef typename detail::get_types<Swapped>::type NewTypes;

    public:
        TempSwapped(const Comps & comps, const NewType & comp) :
            m_comps(comps), m_comp(comp) {}

        template <typename ComponentName>
        const typename detail::TypeForName<ComponentName, Names, NewTypes>::type & get() const {
            return get(detail::WrapToConstructable<ComponentName>());
        }
    private:

        template <typename ComponentName>
        auto get(detail::WrapToConstructable<ComponentName> w) const ->
        decltype(std::declval<const Comps>().template get<ComponentName>()) {
            return m_comps.template get<ComponentName>();
        }

        const NewType & get(detail::WrapToConstructable<Name>) const {
            return m_comp;
        }

        const Comps & m_comps; 
        const NewType & m_comp; 
    };
}


template <typename Name, typename NewType, typename Names, typename Types> 
typename SwapType<Name, NewType, detail::Components<Names, Types> >::type
swap(NewType comp, detail::Components<Names, Types> components){
    typedef detail::Components<Names, Types> Comps;
    typedef typename SwapType<Name, NewType, Comps>::type Swapped;
//    detail::TempSwapped<Name, NewType, Names, Types> st(components, comp);

    Swapped resComponents(
              detail::TempSwapped<Name, NewType, Names, Types>
                          (components, comp));

    return std::move(resComponents);
}

} //data_structures
}//paal
#endif /* COMPONENTS_HPP */
