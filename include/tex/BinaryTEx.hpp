#ifndef _BINARYTEX_HPP
#define _BINARYTEX_HPP

/// \file BinaryTEx.hpp
///
/// \brief Header file defining basic properties of Binary Template EXpressions

#include <tex/BaseTEx.hpp>

namespace SUNphi
{
  /// Defines the BinaryTEx type traits
  DEFINE_BASE_TYPE(BinaryTEx,: public BaseTEx);
  
  // Defines the check for a member type "ref1"
  DEFINE_HAS_MEMBER(ref1);
  
  // Defines the check for a member type "ref2"
  DEFINE_HAS_MEMBER(ref2);
  
  /// Defines the check for a Binary TEx
#define STATIC_ASSERT_IS_BINARY_TEX(...)		\
  STATIC_ASSERT_IS_TEX(__VA_ARGS__);			\
  STATIC_ASSERT_HAS_MEMBER(ref1,__VA_ARGS__);		\
  STATIC_ASSERT_HAS_MEMBER(ref2,__VA_ARGS__)
  
  /// Binary TEmplate Expression
  template <typename T>
  struct BinaryTEx :
    public TEx<T>,
    public BaseBinaryTEx
  {
    PROVIDE_CRTP_CAST_OPERATOR(T);
  };
  
  /// Set the assignability according to the references 1 and 2
#define ASSIGNABLE_ACCORDING_TO_REF_1_2					\
  IS_ASSIGNABLE_ATTRIBUTE(/*! This TEx can be assigned according to the references 1 and 2 */, \
			  RemoveReference<decltype(ref1)>::isAssignable and \
			  RemoveReference<decltype(ref2)>::isAssignable)

}

#endif
