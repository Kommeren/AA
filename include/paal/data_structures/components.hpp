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

#include <boost/mpl/zip_view.hpp>
#include <boost/mpl/fold.hpp>
#include <boost/mpl/map.hpp>
#include <boost/mpl/find.hpp>
#include <boost/fusion/container/map.hpp>
#include <boost/fusion/include/map.hpp>
#include <boost/fusion/view/zip_view.hpp>
#include <boost/fusion/include/zip_view.hpp>
#include <boost/fusion/include/mpl.hpp>
#include <boost/fusion/sequence/intrinsic/at_key.hpp>
#include <boost/fusion/include/at_key.hpp>
#include <boost/preprocessor/repetition/enum.hpp>


namespace paal {
namespace data_structures {

struct NoDefault;


namespace detail {
    template <typename Names, typename Types>
    class Components {
        static const int N = boost::mpl::size<Names>::type::value;
        static_assert(boost::mpl::size<Types>::type::value == N, "Incrrect number of parameters");
        typedef typename boost::mpl::zip_view<boost::mpl::vector<Names, Types>> NamesWithTypes;
        
        template <template <class, class> class Pair>
        struct NamesWithTypesToPairs {
        typedef typename boost::mpl::transform_view <
                                NamesWithTypes,
                                boost::mpl::unpack_args<Pair<boost::mpl::_1, 
                                                             boost::mpl::_2>>
                             >::type type;
        };

        typedef typename boost::mpl::fold<
                typename NamesWithTypesToPairs<boost::mpl::pair>::type,
                boost::mpl::map<>,
                boost::mpl::insert<boost::mpl::_1, boost::mpl::_2>
            >::type NameTypeMap;

        template <typename Name>
        struct TypeForName {
            typedef typename boost::mpl::at<NameTypeMap, Name>::type type;
        };

        //TODO we do not support object which are not default constructible
        template <int k, typename Component, typename... ComponentTypesPrefix>
        void setComonentsPartial(Component component, ComponentTypesPrefix... components) {
            typedef typename boost::mpl::at<Names, boost::mpl::int_<k>>::type Name;
            boost::fusion::at_key<Name>(m_components) = component;
            setComonentsPartial<k+1>(components...);
        }
       
        //boundary condition
        template <int k>
        void setComonentsPartial() {}

    public:
        typedef typename boost::fusion::result_of::as_map<
                typename NamesWithTypesToPairs<boost::fusion::pair>::type
            >::type ComponentsMap;
        
        Components() = default;
        
        Components(Components &&) = default;

        Components(const Components &) = default;
        
        Components& operator=(const Components &) = default;

        Components& operator=(Components &&) = default;

        template <typename... ComponentTypesPrefix>
        Components(ComponentTypesPrefix... components) {
           setComonentsPartial<0>(components...); 
        }
        
        template <typename ComponentName, typename... Args>
        auto call(Args&&... args) ->
        decltype(std::declval<typename TypeForName<ComponentName>::type>()(std::forward<Args>(args)...)) {
            return boost::fusion::at_key<ComponentName>(m_components)(std::forward<Args>(args)...);
        }

        template <typename ComponentName>
        void set(const typename TypeForName<ComponentName>::type & comp) {
            boost::fusion::at_key<ComponentName>(m_components) = comp;
        }
        
        template <typename ComponentName>
        typename boost::fusion::result_of::at_key<ComponentsMap, ComponentName>::type & get() {
            return boost::fusion::at_key<ComponentName>(m_components);
        }
        
        template <typename ComponentName>
        const typename boost::fusion::result_of::at_key<const ComponentsMap, ComponentName>::type & get() const {
            return boost::fusion::at_key<ComponentName>(m_components);
        }

    private:

        ComponentsMap m_components;
    };
};


//implementation on variadic templates is imposible because of
//week support for type detection for inner template classes
template <typename... ComponentNames>
class Components {
    typedef typename boost::mpl::vector<ComponentNames...> Names;
public:
    template <typename... ComponentTypes>
    using type = typename detail::Components<Names, typename boost::mpl::vector<ComponentTypes...>>;
};


template <typename Name, typename NewType, typename Components> class SwapType;

template <typename Name, typename NewType, typename Names, typename Types> 
class SwapType<Name, NewType, detail::Components<Names, Types>> {
public:
    typedef typename boost::mpl::find<Names, Name>::type::pos pos; // position to insert
    typedef typename boost::mpl::begin<Types>::type TypesBegin; // begin iterator
    typedef typename boost::mpl::advance<TypesBegin, pos>::type TypeIter; // iterator on position
    typedef typename boost::mpl::erase<Types, TypeIter>::type TypesErased; // removed old element
    typedef typename boost::mpl::begin<TypesErased>::type ErasedBegin; // begin of new collection with erased element
    typedef typename boost::mpl::advance<ErasedBegin, pos>::type TypeIterErased; // correct place in collection with erased element
    typedef typename boost::mpl::insert<TypesErased, TypeIterErased, NewType>::type TypesSwapped; // insert new element  (final collection) 
    typedef detail::Components<Names, TypesSwapped> type;
};


namespace detail {
    template <typename T>
    struct WrapToConstructable {
        typedef T type;
    };

    template <typename OldComps, typename NewComps>
    class Rewrite {
    public:
        Rewrite(const OldComps & old, NewComps & nw) : 
            m_old(old), m_new(nw) {}

        template <typename NameWrap>
        void operator()(NameWrap a) {
            typedef typename NameWrap::type Name;
            m_new.template set<Name>(std::move(m_old.template get<Name>()));
        }

    private:
        const OldComps & m_old;
        NewComps & m_new;
    };
    
    template <typename OldComps, typename NewComps>
    Rewrite<OldComps, NewComps>
    make_Rewrite(OldComps & old, NewComps & nw) {
        return Rewrite<OldComps, NewComps>(old, nw);
    }

    struct ToConstructable {
        template <typename T>
        struct apply {
            typedef WrapToConstructable<T> type;
        };
    };
}

template <typename Name, typename NewType, typename Names, typename Types> 
typename SwapType<Name, NewType, detail::Components<Names, Types> >::type
swap(NewType comp, detail::Components<Names, Types> components){
    typename SwapType<Name, NewType, detail::Components<Names, Types> >::type resComponents;
    
    typedef typename boost::mpl::find<Names, Name>::type pos; 
    typedef typename boost::mpl::erase<Names, pos>::type NamesErased; // removed old element

    boost::mpl::for_each(detail::make_Rewrite(components, resComponents), 
                         static_cast<NamesErased*>(0), 
                         static_cast<detail::ToConstructable*>(0));
    resComponents.template set<Name>(comp);

    return std::move(resComponents);
}

} //data_structures
}//paal
#endif /* COMPONENTS_HPP */
