#ifndef FRACTION_HPP
#define FRACTION_HPP

namespace paal {
namespace data_structures {

   template<class A, class B>
   struct Fraction {
         A num;
         B den;
         Fraction(A num, B den) : num(num), den(den) {}
   };

   template<class A, class B>
   bool operator<(const Fraction<A, B>& f1, const Fraction<A, B>& f2) {
      return f1.num * f2.den < f2.num * f1.den;
   }

}//!data_structures
}//!paal

#endif // FRACTION_HPP
