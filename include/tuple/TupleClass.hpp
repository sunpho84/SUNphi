#ifndef _TUPLECLASS_HPP
#define _TUPLECLASS_HPP

/// \file TupleClass.hpp
///
/// \brief Define Tuple

#include <tuple>

#include <ints/IntSeq.hpp>
#include <metaprogramming/TypeTraits.hpp>

namespace SUNphi
{
  /// Define Tuple, a list of arbitrary types.
  ///
  /// Directly aliasing the std library.
  template <class...Tp>
  using Tuple=
    std::tuple<Tp...>;
  
  /////////////////////////////////////////////////////////////////
  
  /// Check that a type is a tuple
  ///
  /// Generic false case
  template <class T>
  struct _isTuple : public FalseType
  {
  };
  
  /// Check that a type is a tuple
  ///
  /// Specialization for a true tuple
  template <class...Tp>
  struct _isTuple<Tuple<Tp...>> : public TrueType
  {
  };
  
  /// Check that a type is a tuple
  ///
  /// Gives visibility to the internal implementation
  template <class T>
  [[maybe_unused]]
  constexpr int isTuple=
    _isTuple<Unqualified<T>>::value;
  
  /// Assert if the type is not a Tuple
#define STATIC_ASSERT_IS_TUPLE(T)			\
  static_assert(isTuple<T>,"Type is not a tuple")
  
  /// Constrain the class T to be a Tuple
  template <class T>
  struct ConstrainIsTuple
  {
    STATIC_ASSERT_IS_TUPLE(T);
  };
  
  /////////////////////////////////////////////////////////////////
  
  /// Returns the type of the element I of the Tuple Tp
  template <int I,                           // Order of the type in the Tuple
	    typename Tp,                     // Tuple type
	    typename=EnableIf<isTuple<Tp>>>  // Force Tp to be a Tuple
  using TupleElementType=
    typename std::tuple_element<I,Tp>::type;
  
  /////////////////////////////////////////////////////////////////
  
  /// Total number of elements in a \c Tuple
  template <typename T>
  [[maybe_unused]]
  constexpr int tupleSize=
    std::tuple_size<Unqualified<T>>::value;
  
  /////////////////////////////////////////////////////////////////
  
  /// Define a Variadic type taking all types of a \c tuple as list of parameters
  ///
  /// Example:
  ///
  /// \code
  /// template <class...Tp>
  /// struct Test
  /// {
  ///    int size=sizeof...(Tp);
  /// };
  ///
  /// DEFINE_VARIADIC_TYPE_FROM_TUPLE(Test);
  ///
  /// int size=TestFromTuple<Tuple<int,char>>::size; //2
  ///
  /// \endcode
#define DEFINE_VARIADIC_TYPE_FROM_TUPLE(TYPE)				\
									\
  /*! Defines a variadic type \c TYPE taking the same types of a \c Tuple */ \
  /*!                                                                     */ \
  /*! Forward declaration of the internal implementation                  */ \
  template <class TP,							\
	    class=FalseType>						\
  struct _ ## TYPE ## FromTuple;					\
  									\
  /*! Defines a variadic type \c TYPE taking the same types of a \c Tuple */ \
  /*!                                                                     */ \
  /*! Actual iplementation                                                */ \
  template <class...Tp>							\
  struct _ ## TYPE ## FromTuple<Tuple<Tp...>>				\
  {									\
  									\
    /*! Type which is defined out of the tuple */			\
    using type=TYPE<Tp...>;						\
  };									\
									\
  /*! Defines a variadic type \c TYPE taking the same types of a \c Tuple */ \
  /*!                                                                     */ \
  /*! Gives visibility to the internal implementation                     */ \
  template <class TP,							\
	    class=ConstrainIsTuple<TP>>					\
  using TYPE ## FromTuple=typename _ ## TYPE ## FromTuple<TP>::type
  
  /////////////////////////////////////////////////////////////////
  
  /// Counts the same types
  ///
  /// Single pair of types case - forbids implementation
  template <class T1,
	    class T2,
	    class=FalseType>
  [[ maybe_unused ]]
  static constexpr int _nOfTypeInTuple=
    0;
  
  /// Counts the same type
  ///
  /// Counts the occurrency of type T inside a tuple
  template <class T,
	    class...Tp>
  static constexpr int _nOfTypeInTuple<T,Tuple<Tp...>> =
    hSum<isSame<T,Tp>...>;
  
  /// Counts the same type
  ///
  /// Gives external visibility to the implementation
  template <class T,
	    class TP,
	    class=ConstrainIsTuple<TP>>
  [[ maybe_unused ]]
  static constexpr int nOfTypeInTuple=
    _nOfTypeInTuple<T,TP>;
  
  /////////////////////////////////////////////////////////////////
  
  /// Count the number of different types in a \c Tuple
  ///
  /// Generic type - forbids instantiation
  template <class T,
	    class=ConstrainIsTuple<T>>
  [[ maybe_unused ]]
  static constexpr int nDiffTypesInTuple=
    0;
  
  /// Count the number of different types in a \c Tuple
  ///
  /// Real tuple case
  template <class...Tp>
  static constexpr int nDiffTypesInTuple<Tuple<Tp...>> =
    IntSeq<(nOfTypeInTuple<Tp,Tuple<Tp...>> ==1)...>::hSum;
  
  /////////////////////////////////////////////////////////////////
  
  /// Check whether all types of a \c Tuple are different
  template <class T,
	    class=ConstrainIsTuple<T>>
  [[ maybe_unused ]]
  static constexpr bool tupleTypesAreAllDifferent=
    (nDiffTypesInTuple<T> ==tupleSize<T>);
  
  /// Assert if the \c Tuple contains multiple times a given type
#define STATIC_ASSERT_TUPLE_TYPES_ARE_ALL_DIFFERENT(T)	\
  static_assert(tupleTypesAreAllDifferent<T>,"Types in the tuple are not all different")
  
  /// Constrain all types in the tuple to be different
  template <class T,class=ConstrainIsTuple<T>>
  struct ConstrainTupleTypesAreAllDifferent
  {
    STATIC_ASSERT_TUPLE_TYPES_ARE_ALL_DIFFERENT(T);
  };
  
  /////////////////////////////////////////////////////////////////
  
  /// Return true if \c Tuple Tp contains the type T
  ///
  /// Internal implementation
  template <typename T,     // Type to be searched
	    typename...Tp>  // Tuple in which to search
  constexpr bool _tupleHasType(T,
			       Tuple<Tp...>)
  {
    return (isSame<T,Tp> || ...);
  }
  
  /// Return true if \c Tuple Tp contains the type T
  template <typename T,   // Type to be searched
	    typename Tp,  // Tuple to search
	    typename=ConstrainIsTuple<Tp>>
  [[ maybe_unused ]]
  constexpr bool tupleHasType=
    _tupleHasType(T{},Tp{});
  
  /// Assert if the type \c T is not in the types of tuple \c TP
#define STATIC_ASSERT_TUPLE_HAS_TYPE(T,				\
				     TP)			\
  static_assert(tupleHasType<T,TP>,"Searched type not found")
  
  /// Constrain a type T to be contained in a \c Tuple
  template <class T,
	    class TP,
	    class=ConstrainIsTuple<TP>>
  struct ConstrainTupleHasType
  {
    STATIC_ASSERT_TUPLE_HAS_TYPE(T,TP);
  };
  
  /// Assert if the type \c T is in the types of tuple \c TP
#define STATIC_ASSERT_TUPLE_HAS_NOT_TYPE(T,TP)			\
  static_assert(not tupleHasType<T,TP>,"Searched type found")
  
  /// Constrain a type T to be not contained in a \c Tuple
  template <class T,
	    class TP,
	    class=ConstrainIsTuple<TP>>
  struct ConstrainTupleHasNotType
  {
    STATIC_ASSERT_TUPLE_HAS_NOT_TYPE(T,TP);
  };
  
  /////////////////////////////////////////////////////////////////
  
  /// Checks that a tuple is contained into another one
  ///
  /// Internal implementation
  template <typename...ToBeSearchedTypes,
	    typename...ContainingTypes>
  constexpr bool _tupleHasTypes(Tuple<ToBeSearchedTypes...>, ///< Types to be searched
				Tuple<ContainingTypes...>)   ///< Types in which to search
  {
    return (tupleHasType<ToBeSearchedTypes,Tuple<ContainingTypes...>> && ...);
  }
  
  /// Checks that a tuple is contained into another one
  ///
  /// Gives visibility to the internal implementation
  template <typename TpToSearch,
	    typename TpContaining>
  [[ maybe_unused ]]
  constexpr bool tupleHasTypes=
    _tupleHasTypes(TpToSearch{},TpContaining{});
}

#endif
