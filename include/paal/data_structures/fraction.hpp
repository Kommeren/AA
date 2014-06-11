#ifndef FRACTION_HPP
#define FRACTION_HPP

namespace paal {
namespace data_structures {

    /**
     * @brief simple class to represent fraction
     *
     * @tparam A
     * @tparam B
     */
   template<class A, class B>
   struct Fraction {
         ///numerator
         A num;
         ///denominator
         B den;
         ///constructor
         Fraction(A num, B den) : num(num), den(den) {}
   };

   /**
    * @brief operator<
    *
    * @tparam A
    * @tparam B
    * @param f1
    * @param f2
    *
    * @return
    */
   template<class A, class B>
   bool operator<(const Fraction<A, B>& f1, const Fraction<A, B>& f2) {
      return f1.num * f2.den < f2.num * f1.den;
   }

   /**
    * @brief operator>
    *
    * @tparam A
    * @tparam B
    * @param f1
    * @param f2
    *
    * @return
    */
   template<class A, class B>
   bool operator>(const Fraction<A, B>& f1, const Fraction<A, B>& f2) {
      return f2 < f1;
   }

   /**
    * @brief operator<=
    *
    * @tparam A
    * @tparam B
    * @param f1
    * @param f2
    *
    * @return
    */
   template<class A, class B>
   bool operator<=(const Fraction<A, B>& f1, const Fraction<A, B>& f2) {
      return !(f2 < f1);
   }

   /**
    * @brief operator>=
    *
    * @tparam A
    * @tparam B
    * @param f1
    * @param f2
    *
    * @return
    */
   template<class A, class B>
   bool operator>=(const Fraction<A, B>& f1, const Fraction<A, B>& f2) {
      return !(f1 < f2);
   }

}//!data_structures
}//!paal

#endif // FRACTION_HPP
