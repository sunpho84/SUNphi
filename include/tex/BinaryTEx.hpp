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
  
  /// Provide the reference to the objects
#define PROVIDE_BINARY_TEX_REFS			\
  PROVIDE_TEX_REF(1);				\
  PROVIDE_TEX_REF(2)
  
  /// Create a simple builder with a name and a BINARY_TEX returned type
#define SIMPLE_BINARY_TEX_BUILDER(BUILDER,    /*!< Name of builder fun           */ \
				  BINARY_TEX) /*!< Name of the BinaryTEx to build */ \
  /*! Simple BINARY_TEX builder called BUILDER */			\
  /*!                                          */			\
  /*! Plain BINARY_TEX getting a pair of plain TEx                    */ \
  template <typename T1, 	    /* Type of the first TEx to get   */ \
	    typename T2, 	    /* Type of the second TEx to get  */ \
	    SFINAE_WORSEN_DEFAULT_VERSION_TEMPLATE_PARS>		\
  BINARY_TEX<T1,T2> BUILDER(T1&& tex1,   /*!< First TEx to act upon  */	\
			    T2&& tex2,   /*!< Second TEx to act upon */	\
			    SFINAE_WORSEN_DEFAULT_VERSION_ARGS)		\
  {									\
    SFINAE_WORSEN_DEFAULT_VERSION_ARGS_CHECK;				\
  									\
    return BINARY_TEX<T1,T2>(forw<T1>(tex1),forw<T2>(tex2));		\
  }									\
  SWALLOW_SEMICOLON_AT_GLOBAL_SCOPE
  
  /// Set aliasing according to the isAliasing of references 1 and 2
  /// \todo enforce cehck only with TensClass
#define FORWARD_IS_ALIASING_TO_REF_1_2			\
  /*! Forward aliasing check to the references */	\
  template <typename Tref>				\
  bool isAliasing(const Tref& alias) const		\
  {							\
    return						\
      ref1.isAliasing(alias) or				\
      ref2.isAliasing(alias);				\
  }							\
  SWALLOW_SEMICOLON_AT_CLASS_SCOPE
  
  /// Set the assignability according to the references 1 and 2
#define ASSIGNABLE_ACCORDING_TO_REF_1_2					\
  IS_ASSIGNABLE_ATTRIBUTE(/*! This TEx can be assigned according to the references 1 and 2 */, \
			  RemoveReference<decltype(ref1)>::isAssignable and \
			  RemoveReference<decltype(ref2)>::isAssignable)

}

#endif
