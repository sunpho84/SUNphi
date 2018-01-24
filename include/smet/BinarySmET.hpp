#ifndef _BINARYSMET_HPP
#define _BINARYSMET_HPP

/// \file BinarySmET.hpp
///
/// \brief Header file defining basic properties of Binary SmET

#include <smet/BaseSmET.hpp>

namespace SUNphi
{
  /// Defines the BinarySmET type traits
  DEFINE_BASE_TYPE(BinarySmET,: public BaseSmET);
  
  // Defines the check for a member type "ref1"
  DEFINE_HAS_MEMBER(ref1);
  
  // Defines the check for a member type "ref2"
  DEFINE_HAS_MEMBER(ref2);
  
  /// Defines the check for a Binary SmET
#define STATIC_ASSERT_IS_BINARY_SMET(...)		\
  STATIC_ASSERT_IS_SMET(__VA_ARGS__);			\
  STATIC_ASSERT_HAS_MEMBER(ref1,__VA_ARGS__);		\
  STATIC_ASSERT_HAS_MEMBER(ref2,__VA_ARGS__)
  
  /// Binary SmET
  template <typename T>
  struct BinarySmET :
    public SmET<T>,
    public BaseBinarySmET
  {
    PROVIDE_CRTP_CAST_OPERATOR(T);
  };
  
  /// Provide the reference to the objects
#define PROVIDE_BINARY_SMET_REFS			\
  PROVIDE_SMET_REF(1);				\
  PROVIDE_SMET_REF(2)
  
  /// Create a simple builder with a name and a BINARY_SMET returned type
#define SIMPLE_BINARY_SMET_BUILDER(BUILDER,    /*!< Name of builder fun           */ \
				  BINARY_SMET) /*!< Name of the BinarySmET to build */ \
  /*! Simple BINARY_SMET builder called BUILDER */			\
  /*!                                          */			\
  /*! Plain BINARY_SMET getting a pair of plain SmET                    */ \
  template <typename T1, 	    /* Type of the first SmET to get   */ \
	    typename T2, 	    /* Type of the second SmET to get  */ \
	    SFINAE_WORSEN_DEFAULT_VERSION_TEMPLATE_PARS>		\
  BINARY_SMET<T1,T2> BUILDER(T1&& smet1,   /*!< First SmET to act upon  */	\
			    T2&& smet2,   /*!< Second SmET to act upon */	\
			    SFINAE_WORSEN_DEFAULT_VERSION_ARGS)		\
  {									\
    SFINAE_WORSEN_DEFAULT_VERSION_ARGS_CHECK;				\
  									\
    return BINARY_SMET<T1,T2>(forw<T1>(smet1),forw<T2>(smet2));		\
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
  IS_ASSIGNABLE_ATTRIBUTE(/*! This SmET can be assigned according to the references 1 and 2 */, \
			  RemoveReference<decltype(ref1)>::isAssignable and \
			  RemoveReference<decltype(ref2)>::isAssignable)

}

#endif
