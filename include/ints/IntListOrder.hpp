#ifndef _INTLISTORDER_HPP
#define _INTLISTORDER_HPP

/// \file IntListOrder.hpp
///
/// \brief Implements order check and operations on integer list

namespace SUNphi
{
  /// Compare the order of at least two integers
  ///
  /// Return true if First<Second or First==Second (only if
  /// AlsoEqual==true). Call iteratively itself if more than two
  /// arguents are passed.
  template <bool AlsoEqual, // Flag to return true if First==Second
	    int First,      // First element to compare
	    int Second,     // Second element to compare
	    int...Tail>     // Other elements to compare
  constexpr bool _areOrderedMin2Ints();
  
  /// Compare a list of integer
  ///
  /// If less than two elements are pased, returns true. Otherwise
  /// call the routine which operates on at least two integers
  template <bool AlsoEqual, // Flag to return true if First==Second
	    int...Ints>     // List of integers to compare
  constexpr bool _areOrdered()
  {
    if constexpr(sizeof...(Ints)<2)
       return true;
    else
      return _areOrderedMin2Ints<AlsoEqual,Ints...>();
  }
  
  // Actual implementation (see above for comments)
  template <bool AlsoEqual,
	    int First,
	    int Second,
	    int...Tail>
  constexpr bool _areOrderedMin2Ints()
  {
    constexpr bool compa=AlsoEqual?(First<=Second):(First<Second);
    
    return compa and _areOrdered<AlsoEqual,Second,Tail...>();
  }
  
  /// Returns true if all elements are ascending-ordered and different
  template <int...Ints>
  [[ maybe_unused ]]
  constexpr bool areOrdered=
    _areOrdered<true,Ints...>();
  
  /// Returns true if all elements are weak ascending-ordered
  template <int...Ints>
  [[ maybe_unused ]]
  constexpr bool areOrderedAndDifferent=
    _areOrdered<false,Ints...>();
  
  /////////////////////////////////////////////////////////////////////////
  
  /// Constant used to parse first-occurrency case
#define FIRST_OF 0
  
  /// Constant used to parse last-occurrency case
#define LAST_OF 1
  
  /// Defines a searcher named NAME making the comparison COMPA
#define DEFINE_SEARCH(DESCRIPTION,FIRST_OR_LAST,NAME,COMPA)		\
  DESCRIPTION								\
  /*!                                                         */	\
  /*! Internal implementation                                 */	\
  template <int I,       /* Element to search                 */	\
	    int Pos,     /* Position currently reached        */	\
	    int NParsed, /* Number of elements already parsed */	\
	    int Head,    /* First component to be searched    */	\
	    int...Tail>  /* Other components                  */	\
  constexpr int _ ## NAME()						\
  {									\
    constexpr bool matching=(Head COMPA I);				\
    constexpr bool exitAtFirst=(FIRST_OR_LAST==FIRST_OF);		\
    constexpr bool emptyTail=(sizeof...(Tail)==0);			\
    constexpr int newPos=(matching?NParsed:Pos);			\
    									\
    if constexpr((not emptyTail) and not (matching and exitAtFirst))	\
      return _ ## NAME<I,newPos,NParsed+1,Tail...>();	                \
    else								\
      if constexpr(matching and (exitAtFirst or emptyTail))		\
		    return newPos;					\
      else								\
	return newPos;							\
  }									\
									\
  DESCRIPTION								\
  /*!                                         */			\
  /*! General case                            */			\
  template <int I,       /* Element to search */			\
	    int...Ints>  /* Components        */			\
  [[ maybe_unused ]]							\
  constexpr int NAME=							\
    _ ## NAME<I,sizeof...(Ints),0,Ints...>();				\
									\
  DESCRIPTION								\
  /*!                                         */			\
  /*! Empty sequence case                     */			\
  template <int I>       /* Element to search */			\
  constexpr int NAME<I> =						\
    0
  
  DEFINE_SEARCH(/*! Returns the position of the first element different from I */,FIRST_OF,firstNon,!=);
  DEFINE_SEARCH(/*! Returns the position of the last element different from I */,LAST_OF,lastNon,!=);
  
  DEFINE_SEARCH(/*! Returns the position of the first element equal to I */,FIRST_OF,firstEq,==);
  DEFINE_SEARCH(/*! Returns the position of the last element equal to I */,LAST_OF,lastEq,==);
  
  DEFINE_SEARCH(/*! Returns the position of the first element smaller than I */,FIRST_OF,firstSmaller,<);
  DEFINE_SEARCH(/*! Returns the position of the last element smaller than I */,LAST_OF,lastSmaller,<);
  
  DEFINE_SEARCH(/*! Returns the position of the first element larger than I */,FIRST_OF,firsLarger,>);
  DEFINE_SEARCH(/*! Returns the position of the last element larger than I */,LAST_OF,lastLarger,>);
  
#undef DEFINE_FIRST_OF
#undef DEFINE_LAST_OF
}

#endif
