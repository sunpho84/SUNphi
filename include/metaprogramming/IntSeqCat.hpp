#ifndef _INTSEQCAT_HPP
#define _INTSEQCAT_HPP

/// \file IntSeq.hpp
///
/// \brief Concatenate IntSeq

#include <metaprogramming/IntSeq.hpp>

namespace SUNphi
{
  template <class...T>
  struct IntSeqCatImpl;
  
  //! single IntSeq
  template <int...Ints>
  struct IntSeqCatImpl<IntSeq<Ints...>>
  {
    typedef IntSeq<Ints...> type;
  };
  
  //! concatenate multiple occurency
  template <int...Ints1,int...Ints2,class...T>
  struct IntSeqCatImpl<IntSeq<Ints1...>,IntSeq<Ints2...>,T...>
  {
    using Nested=IntSeq<Ints1...,Ints2...>;
    typedef typename IntSeqCatImpl<Nested,T...>::type type;
  };
  
  //! wrap the tuple type catter
  template <class...T>
  using IntSeqCatT=typename IntSeqCatImpl<T...>::type;
}

#endif
