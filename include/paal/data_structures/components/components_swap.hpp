/**
 * @file components_swap.hpp
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
 * @brief Generic version of SwapType
 *
 * @tparam Name
 * @tparam NewType
 * @tparam Components
 */
template <typename Name, typename NewType, typename Components> class SwapType;

/**
 * @class SwapType
 * @brief Returns type of  Components<Names, Types>, with Type for Name change to NewType
 *
 * @tparam Name name of the changed type
 * @tparam NewType new type for Name
 * @tparam Names names list
 * @tparam Types old types list
 */
template <typename Name, typename NewType, typename Names, typename Types> 
class SwapType<Name, NewType, detail::Components<Names, Types>> {
    static const int p =  pos<Name, Names>::value; // position to insert
    typedef typename replace_at_pos<p, NewType, Types>::type TypesSwapped;
public:
    typedef detail::Components<Names, TypesSwapped> type;
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
     * @class TempSwappedComponents
     * @brief This class behavies like partial Components<Names, Types>, 
     *        with type for Name chanche to Type
     *
     * @tparam Name changed name
     * @tparam NewType new type
     * @tparam Names all names
     * @tparam Types aol types
     */
    template <typename Name, typename NewType, typename Names, typename Types> 
    class TempSwappedComponents {
        typedef detail::Components<Names, Types> Comps;
        typedef typename SwapType<Name, NewType, Comps>::type Swapped;
        typedef typename detail::get_types<Swapped>::type NewTypes;

    public:
        TempSwappedComponents(const Comps & comps, const NewType & comp) :
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


/**
 * @brief This function, for a specific Name, swaps compoonent in the Components class.
 *        The comonent should have deifferent type than prevoius component for this Name 
 *        (If the type is the same, set member function from Components class chould be used).
 *        The function returns Components class fo type SwapType<Name, NewType, OldComponents >::type.
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
typename SwapType<Name, NewType, detail::Components<Names, Types> >::type
swap(NewType comp, detail::Components<Names, Types> components){
    typedef detail::Components<Names, Types> Comps;
    typedef typename SwapType<Name, NewType, Comps>::type Swapped;

    return Swapped(
              detail::TempSwappedComponents<Name, NewType, Names, Types>
                          (components, comp));
}

} //data_structures
} //paal

#endif /* COMPONENTS_SWAP_HPP */
