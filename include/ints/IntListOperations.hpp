#ifndef _INT_LIST_OPERATIONS_HPP
#define _INT_LIST_OPERATIONS_HPP

/// \file IntListOperations.hpp
///
/// \brief Implements operations over list of integers

namespace SUNphi
{
  /// Sum of all integers
  ///
  /// Internal implementation
  template <int...Ints>
  constexpr int _hSum()
  {
    if constexpr(sizeof...(Ints)==0)
      return 0;
    else
      return (Ints + ...);
  }
  
  /// Sum of all integers
  template <int...Ints>
  [[ maybe_unused ]]
  constexpr int hSum=_hSum<Ints...>();
  
  /////////////////////////////////////////////////////////////////
  
  /// Sum of all integers up to I (excluded)
  template <int I,
	    int Head=0,
	    int...Tail>
  [[ maybe_unused ]]
  constexpr int hSumFirst=
    (I>0)*Head+hSumFirst<I-1,Tail...>;
  
  /// Sum of all integers up to I (excluded),unary case
  template <int I,
	    int Head>
  constexpr int hSumFirst<I,Head> =
    (I>0)*Head;
  
  /////////////////////////////////////////////////////////////////
  
  /// Product of all integers
  ///
  /// Internal implementation
  template <int...Ints>
  [[ maybe_unused ]]
  constexpr int _hMul()
  {
    if constexpr(sizeof...(Ints)==0)
      return 1;
    else
      return (Ints * ...);
  }
  
  /// Product of all integers
  template <int...Ints>
  [[ maybe_unused ]]
  constexpr int hMul=_hMul<Ints...>();
  
  /////////////////////////////////////////////////////////////////
  
#define DEFINE_SEARCH_MIN_OR_MAX(NAME,		\
				 DESCR,		\
				 OPER)		\
  /*! DESCR of all integers   */		\
  /*!                         */		\
  /*! Internal implementation */		\
  template <int In,				\
	    int Head,				\
	    int...Tail>				\
  int _ ## NAME ## OfList()			\
  {						\
    /* Binary comparison */			\
    constexpr int Tmp=(In OPER Head)?In:Head;	\
    						\
    /* Decide to return or nest */		\
    if constexpr(sizeof...(Tail)==0)		\
      return Tmp;			        \
    else					\
      return _ ## NAME ## OfList<Tmp,Tail...>;	\
  }						\
  						\
  /*! DESCR of all integers */			\
  template <int Head,				\
	    int...Tail>				\
  [[ maybe_unused ]]				\
  constexpr int NAME ## OfList=			\
    _ ## NAME ## OfList<Head,Head,Tail...>()
    
  DEFINE_SEARCH_MIN_OR_MAX(max,Maximal,>);
  DEFINE_SEARCH_MIN_OR_MAX(min,Minimal,<);

#undef DEFINE_SEARCH_MIN_OR_MAX
}

#endif
