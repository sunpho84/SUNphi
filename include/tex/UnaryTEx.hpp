#ifndef _UNARYTEX_HPP
#define _UNARYTEX_HPP

/// \file UnaryTEx.hpp
///
/// \brief Header file defining basic properties of Unary Template EXpressions

#include <tex/BaseTEx.hpp>

namespace SUNphi
{
  /// Defines the UnaryTEx type traits
  DEFINE_BASE_TYPE(UnaryTEx,: public BaseTEx);
  
  // Defines the check for a member type "ref"
  DEFINE_HAS_MEMBER(ref);
  
  /// Defines the check for a Unary TEx
#define STATIC_ASSERT_IS_UNARY_TEX(...)			\
  STATIC_ASSERT_IS_TEX(__VA_ARGS__);			\
  STATIC_ASSERT_HAS_MEMBER(ref,__VA_ARGS__)
  
  /// Unary TEmplate Expression
  template <typename T>
  struct UnaryTEx :
    public TEx<T>,
    public BaseUnaryTEx
  {
    PROVIDE_CRTP_CAST_OPERATOR(T);
  };
  
  /////////////////////////////////////////////////////////////////
  
  /// Set the assignability according to the reference
#define ASSIGNABLE_ACCORDING_TO_REF					\
  IS_ASSIGNABLE_ATTRIBUTE(/*! This TEx can be assigned according to the reference */,RemoveReference<decltype(ref)>::isAssignable)
  
  /////////////////////////////////////////////////////////////////
  
  /// Set aliasing according to the isAliasing of reference
  /// \todo enforce cehck only with TensClass
#define FORWARD_IS_ALIASING_TO_REF			\
  /*! Forward aliasing check to the reference */	\
  template <typename Tref>				\
  bool isAliasing(const Tref& alias) const		\
  {							\
    return ref.isAliasing(alias);			\
  }							\
  SWALLOW_SEMICOLON_AT_CLASS_SCOPE
  
  /// Set aliasing according to a passed pointer (provided class member)
#define IS_ALIASING_ACCORDING_TO_POINTER(_p)				\
  /*! Check the aliasing with reference */				\
  template <typename Tref>						\
  bool isAliasing(const Tref& alias) const				\
  {									\
    const void* pAlias=static_cast<const void*>(&alias);		\
    const void* p=static_cast<const void*>(_p);				\
									\
    return pAlias==p;							\
  }									\
  SWALLOW_SEMICOLON_AT_CLASS_SCOPE
}

#endif
