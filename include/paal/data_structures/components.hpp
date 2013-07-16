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

template <typename... ComponentNames>
    class Components {
    static const int N = sizeof...(ComponentNames);
    public:
        template <typename... ComponentTypes>
        class type {
            static_assert(sizeof...(ComponentTypes) == N, "Incrrect number of parameters");
            typedef typename boost::mpl::vector<ComponentNames...> Names;
            typedef typename boost::mpl::vector<ComponentTypes...> Types;
            typedef typename boost::mpl::zip_view<boost::mpl::vector<Names, Types>> NamesWithTypes;
            
            template <template <class, class> class Pair>
            struct NamesWithTypesToPairs {
            typedef typename boost::mpl::transform_view <
                                    NamesWithTypes,
                                    boost::mpl::unpack_args<Pair<boost::mpl::_1, 
                                                                 boost::mpl::_2>>
                                 >::type type;
            };

            typedef typename boost::fusion::result_of::as_map<
                    typename NamesWithTypesToPairs<boost::fusion::pair>::type
                >::type Components;
            
            typedef typename boost::mpl::fold<
                    typename NamesWithTypesToPairs<boost::mpl::pair>::type,
                    boost::mpl::map<>,
                    boost::mpl::insert<boost::mpl::_1, boost::mpl::_2>
                >::type ComponentsMap;

            template <typename Name>
            struct TypeForName {
                typedef typename boost::mpl::at<ComponentsMap, Name>::type type;
            };

            //TODO we do not support object which are not default constructible
            template <int k, typename Component, typename... ComponentTypesPrefix>
            void setComonentsPartial(Component component, ComponentTypesPrefix... components) {
                typedef typename boost::mpl::at<Names, boost::mpl::int_<k>>::type Name;
                boost::fusion::at_key<Name>(m_components) = component;
                setComonentsPartial<k+1>(components...);
            }
            
            template <int k>
            void setComonentsPartial() {}

        public:
            type() = default;
            
            type(ComponentTypes... componets) : m_components(componets...) {}
            

            template <typename... ComponentTypesPrefix>
            type(ComponentTypesPrefix... components) {
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
            typename boost::fusion::result_of::at_key<Components, ComponentName>::type & get() {
                return boost::fusion::at_key<ComponentName>(m_components);
            }
            

        private:

            Components m_components;
        };
    };

} //data_structures
}//paal
#endif /* COMPONENTS_HPP */
