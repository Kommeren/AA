/**
 * @file components_replace.hpp
 * @brief 
 * @author Piotr Wygocki
 * @version 1.0
 * @date 2013-07-24
 */
#ifndef COMPONENTS_SWAP_HPP
#define COMPONENTS_SWAP_HPP 

namespace paal {
namespace data_structures {

/**
 * @brief Generic version of ReplaceType
 *
 * @tparam Name
 * @tparam NewType
 * @tparam Components
 */
template <typename Name, typename NewType, typename Components> class ReplacedType;

/**
 * @class ReplaceType
 * @brief Returns type of  Components<Names, Types>, with Type for Name change to NewType
 *
 * @tparam Name name of the changed type
 * @tparam NewType new type for Name
 * @tparam Names names list
 * @tparam Types old types list
 */
template <typename Name, typename NewType, typename Names, typename Types> 
class ReplacedType<Name, NewType, detail::Components<Names, Types>> {
    static const int p =  pos<Name, Names>::value; // position to insert
    typedef typename replace_at_pos<p, NewType, Types>::type TypesReplace;
public:
    typedef detail::Components<Names, TypesReplace> type;
};

namespace detail {

    /**
     * @brief generic get_types
     *
     * @tparam Comp
     */
    template <typename Comp>
    struct get_types;
    
    /**
     * @class get_types
     * @brief gets types list for Components class
     *
     * @tparam Names
     * @tparam Types
     */
    template <typename Names, typename Types>
        struct get_types<Components<Names, Types>> {
            typedef Types type;
        };
        

        /**
         * @class TempReplaceComponents
         * @brief This class behavies like partial Components<Names, Types>, 
         *        with type for Name chanche to Type
         *
         * @tparam Name changed name
         * @tparam NewType new type
         * @tparam Names all names
         * @tparam Types aol types
         */
        template <typename Name, typename NewType, typename Names, typename Types> 
        class TempReplacedComponents {
            typedef detail::Components<Names, Types> Comps;
            typedef typename ReplacedType<Name, NewType, Comps>::type Replaced;
            typedef typename detail::get_types<Replaced>::type NewTypes;

        public:
            TempReplacedComponents(const Comps & comps, const NewType & comp) :
                m_comps(comps), m_comp(comp) {}

            template <typename ComponentName>
            const typename detail::TypeForName<ComponentName, Names, NewTypes>::type & get() const {
                return get(detail::WrapToConstructable<ComponentName>());
            }
        private:

            template <typename ComponentName>
            auto get(detail::WrapToConstructable<ComponentName>) const ->
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


/**
 * @brief This function, for a specific Name, replaces compoonent in the Components class.
 *        The comonent should have deifferent type than prevoius component for this Name 
 *        (If the type is the same, set member function from Components class chould be used).
 *        The function returns Components class fo type ReplaceType<Name, NewType, OldComponents >::type.
 *        The function creates temporary object wich behaves like result Components 
 *        and creates final object calling special Copy constructor.
 *
 * @tparam Name
 * @tparam NewType
 * @tparam Names
 * @tparam Types
 * @param comp
 * @param components
 *
 * @return 
 */
template <typename Name, typename NewType, typename Names, typename Types> 
typename ReplacedType<Name, NewType, detail::Components<Names, Types> >::type
replace(NewType comp, detail::Components<Names, Types> components){
    typedef detail::Components<Names, Types> Comps;
    typedef typename ReplacedType<Name, NewType, Comps>::type Replaced;

    return Replaced(
              detail::TempReplacedComponents<Name, NewType, Names, Types>
                      (components, comp), CopyTag());
}

} //data_structures
} //paal

#endif /* COMPONENTS_SWAP_HPP */
