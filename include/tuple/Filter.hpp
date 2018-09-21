#ifndef _FILTER_HPP
#define _FILTER_HPP

/// \file Filter.hpp
///
/// \brief Filter elements of a list according to a rule

#include <ints/IntSeqCat.hpp>
#include <ints/Ranges.hpp>

namespace SUNphi
{
  /// Get the position of elements satisfying a rule.
  ///
  /// Internal implementation, forward the declaration.
  template <template <auto> typename F, // Rule to apply
	    typename I,                 // IntSeq-to be
	    auto...>                    // List to filter
  class _FilterVariadicList;
  
  /// Get the position of elements satisfying a rule.
  ///
  /// Internal implementation, recursive case.
  template <template <auto> typename F,  // Rule to apply
	    int...P,                     // Positions
	    auto...T>                    // Elements
  class _FilterVariadicList<F,IntSeq<P...>,T...>
  {
    
    // Force P and T to have the same number of elements
    STATIC_ASSERT_ARE_N_TYPES(sizeof...(P),T);
    
  public:
    
    /// Returned sequence containing the true positions
    using Res=IntSeqCat<Conditional<F<T>::res,
				    IntSeq<P>,
				    IntSeq<>>...>;
  };
  
  /// Get the position of elements satisfying a rule.
  ///
  /// Gets a set of variables as template parameter, and a condition
  /// F, returns the position of those which passes the filter F.
  template <template <auto> typename F,  // Rule to apply
	    auto...T>                    // List of variables
  using FilterVariadicList=
    typename _FilterVariadicList<F,
				 RangeSeq<0,
					  1,
					  sizeof...(T)>,
				 T...>::Res;
  
  /// Get the position of elements of a vector, satisfying a condition
  ///
  /// Forward declaration
  template <template <auto> typename F,    // Rule to apply
	    typename L>                    // Vector
  class _FilterVariadicClass;
  
  /// Get the position of elements of a vector, satisfying a condition
  template <template <auto> typename F,    // Rule to apply
	    template <auto...> typename V, // Kind of vector
	    auto...List>                   // Elements of the vector
  class _FilterVariadicClass<F,V<List...>>
  {
  public:
    
    /// Positions where the condition is true
    using Pos=FilterVariadicList<F,List...>;
    
  };
  
  /// Get the position of elements of a vector, satisfying a condition
  ///
  /// Gets a vector-like incapsulated list of variables, and returns a
  /// SeqInt containing the position of those that satisfy a certain
  /// condition.
  ///
  /// Example:
  ///
  /// \code:
  ///
  /// FilterVariadicClassPos<IsNotNull,IntSeq<0,1,0,10>>; // IntSeq<0,3>;
  ///
  /// \endcode
  template <template <auto> typename F,    // Rule to apply
	    typename L>                    // Class containing the types
  using FilterVariadicClassPos=
    typename _FilterVariadicClass<F,L>::Pos;
  
  /////////////////////////////////////////////////////////////////
  
  /// Filterer which check non-nullity of its parameter
  template <auto I>
  struct IsNotNull
     {
       /// Result of the check
       static constexpr bool res=
	 (I!=0);
     };
}

#endif
