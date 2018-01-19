#ifndef _INTSEQ_HPP
#define _INTSEQ_HPP

/// \file IntSeq.hpp
///
/// \brief Implements a struct holding a sequence of integers

#include <ints/IntListOperations.hpp>
#include <ints/IntListOrder.hpp>
#include <ints/IntListGetEl.hpp>

#include <metaprogramming/TypeTraits.hpp>
#include <metaprogramming/UniversalReferences.hpp>

namespace SUNphi
{
  DEFINE_BASE_TYPE(IntSeq);
  
  /// A struct holding a sequence of integer (similar to stdlib).
  ///
  /// The \c integers are held as parameters of the class. To
  /// intercept them, use the class in a context in which the integer
  /// list are deduced as template parameter, as in this example:
  ///
  /// \code
  /// template <class T>
  /// class PutIntoArrays
  /// {
  /// };
  ///
  /// template <int...Ints>
  /// class PutIntoArrays<IntSeq<Ints...>>
  /// {
  ///    std::array<int,IntSeq<Ints...>::Size> arr{{Ints...}};
  /// };
  /// \endcode
  ///
  /// \tparam Ints the list of integer
  template <int...Ints>
  struct IntSeq :
    public BaseIntSeq
  {
    /// Length of the sequence of integer
    static constexpr int size=sizeof...(Ints);
    
    /// Sum of all elements
    static constexpr int hSum=SUNphi::hSum<Ints...>;
    
    /// Sum of all elements
    template <int I>
    static constexpr int hSumFirst=SUNphi::hSumFirst<I,Ints...>;
    
    /// Take first position wher I does not occur
    template <int I>
    static constexpr int firstNon=SUNphi::firstNon<I,Ints...>;
    
    /// Product of all elements
    static constexpr int hMul=SUNphi::hMul<Ints...>;
    
    /// Defines a new integer sequence incremented by a constant I
    template <int I>
    using Add=IntSeq<(Ints+I)...>;
    
    /// Defines a new integer sequence decreased by a constant I
    template <int I>
    using Sub=Add<-I>;
    
    /// Define a new integer sequence inflated by a constant factor I
    template <int I>
    using Mul=IntSeq<(Ints*I)...>;
    
    /// Define a new integer sequence decreased by a constant factor I
    template <int I>
    using Div=TypeIf<I!=0,Mul<1/I>>;
    
    /// Get the I element of the sequence
    template <int I>
    static constexpr int element=getIntOfList<I,Ints...>;
    
    /// Determine whether the IntSeq elements are ordered or not
    static constexpr int isOrdered=areOrdered<Ints...>;
    
    /// Determine whether the IntSeq elements are ordered and different or not
    static constexpr int isOrderedUnique=areOrderedAndDifferent<Ints...>;
    
    /// Determine whether the element is contained
    template <int El>
    static constexpr bool has=
      firstEq<El,Ints...>!=size;
    
    /////////////////////////////////////////////////////////////////
    
    /// Append the first N id of another IntSeq
    ///
    /// Internal implementation, forward declaration
    template <bool,
	      int N,
	      typename Right>
    struct _AppendFirstN;
    
    /// Append the first N id of another IntSeq
    ///
    /// Internal implementation, taking recursively until N==0
    template <int N,
	      int HeadR,
	      int...TailR>
    struct _AppendFirstN<true,N,IntSeq<HeadR,TailR...>>
    {
      /// Returned type
      using type=typename IntSeq<Ints...,HeadR>::template AppendFirstN<N-1,IntSeq<TailR...>>;
      
    };
    
    /// Append the first N id of another IntSeq
    ///
    /// Internal implementation, terminator N==0
    template <int N,
	      int...R>
    struct _AppendFirstN<false,N,IntSeq<R...>>
    {
      /// Returned type
      using type=IntSeq<R...>;
    };
    
    /// Append the first N id of another IntSeq
    ///
    /// Gives visibility to internal implementation
    template <int N,
	      typename ISeq>
    using AppendFirstN=typename _AppendFirstN<(N>0),N,ISeq>::type;
  };
  
  /////////////////////////////////////////////////////////////////
  
  /// Identifies whether a type is an ordered IntSeq
  ///
  /// General case
  template <typename T>
  [[ maybe_unused ]]
  constexpr bool isOrderedIntSeq=false;
  
  /// Identifies whether a type is an ordered IntSeq
  ///
  /// General case
  template <int...Ints>
  constexpr bool isOrderedIntSeq<IntSeq<Ints...>> =
    IntSeq<Ints...>::isOrdered;
  
  /// Constrain the IntSeq to be ordered
  template <typename T,
	    typename=ConstrainIsIntSeq<T>>
  struct ConstrainIsOrderedIntSeq
  {
    /// Internal type
    using type=T;
    
    static_assert(isOrderedIntSeq<T>,"IntSeq is not ordered");
  };
  
  /// Constrain the IntSeq to be ordered and unique
  template <typename T,
	    typename=ConstrainIsIntSeq<T>>
  struct ConstrainIsOrderedUniqueIntSeq
  {
    /// Internal type
    using type=T;
    
    static_assert(T::isOrderedUnique,"IntSeq is not ordered and unique");
  };
}

#endif
