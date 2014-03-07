/**
 * @file singleton_iterator.hpp
 * @brief
 * @author Robert Rosolek
 * @version 1.0
 * @date 2014-02-27
 */
#ifndef SINGLETON_ITERATOR_HPP
#define SINGLETON_ITERATOR_HPP

#include <boost/iterator/iterator_facade.hpp>
#include <boost/optional/optional.hpp>

namespace paal {
namespace utils {

   /**
    * @brief Iterator to range containing single element.
    * @tparam Elem
    */
   template <typename Elem>
      class SingletonIterator
      : public boost::iterator_facade<
        SingletonIterator<Elem>,
        typename std::decay<Elem>::type,
        boost::forward_traversal_tag,
        Elem
        >
   {
      template<typename E> friend SingletonIterator<E> make_SingletonIteratorBegin(E);
      template<typename E> friend SingletonIterator<E> make_SingletonIteratorEnd();

      /**
       * @brief private constructor. Use make_SingletonIteratorBegin,
       *              make_SingletonIteratorEnd.
       *
       * @param elem
       */
      SingletonIterator() {}

      /**
       * @brief private constructor. Use make_SingletonIteratorBegin,
       *              make_SingletonIteratorEnd.
       *
       * @param elem
       */
      SingletonIterator(Elem elem) : m_elem(elem) {}

      friend class boost::iterator_core_access;

      void increment() { m_elem = boost::none; }

      bool equal(const SingletonIterator& other) const {
         return m_elem == other.m_elem;
      }

      Elem dereference() const { return m_elem.get(); }

      boost::optional<Elem> m_elem;
   };

    /**
     * @brief function to create begin of SingletonIterator
     *
     * @tparam Elem
     * @param elem
     *
     * @return
     */
   template <typename Elem>
      SingletonIterator<Elem> make_SingletonIteratorBegin(Elem elem) {
         return SingletonIterator<Elem>(elem);
      }

    /**
     * @brief function to create end of SingletonIterator
     *
     * @tparam Elem
     * @param elem
     *
     * @return
     */
   template <typename Elem>
      SingletonIterator<Elem> make_SingletonIteratorEnd() {
         return SingletonIterator<Elem>();
      }

} //!utils
} //!paal

#endif /* SINGLETON_ITERATOR_HPP */
