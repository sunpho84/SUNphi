#ifndef _TENSCOMP_HPP
#define _TENSCOMP_HPP

/// \file TensComp.hpp
///
/// \brief Defines the Tensor Components

#include <metaprogramming/CRTP.hpp>
#include <metaprogramming/TypeTraits.hpp>
#include <utility/String.hpp>
#include <utility/Unused.hpp>

namespace SUNphi
{
  /// Defines the BaseTensComp type traits
  ///
  /// Used to specify the underlying kind and number of component of a
  /// tensor component
  DEFINE_BASE_TYPE(TensComp);
  
  /// Constant used to instantiate a Dynamic-size TensComp
  constexpr int DYNAMIC=-1;
  
  /// Tensor Component
  ///
  /// A tensor component is defined by an internal type and the
  /// associated size, which can be known at compile-time or can be
  /// made \c DYNAMIC, to be specified at runtime in the storage class
  template <class T,int Size=DYNAMIC>
  struct TensComp :
    public BaseTensComp
  {
    /// Mapped type
    using type=T;
    
    /// Size of the tensor component (max index value)
    static constexpr int size=Size;
  };
  
  /// Defines a \c TensComp, with name TYPE and max N
#define DEFINE_TENS_COMP_CLASS(TYPE,N)					\
  /*! Tensor component of \c TYPE Kind */				\
  struct TYPE : public TensComp<TYPE,N>					\
  {									\
    PROVIDE_CRTP_CAST_OPERATOR(TYPE);					\
									\
    PROVIDE_NAME(#TYPE);						\
  }
  
/// Define a \c TensComp named \c TYPE with maximal component \c N
///
/// Defines also a binder-instantiator named BINDER, a \c TensKind
/// suffixing TYPE with Kind, and a constat holding the maximal value,
/// named \c CONST_NAME
#define DEFINE_TENS_COMP(BINDER,TYPE,CONST_NAME,N)			\
									\
  /*! Maximal value for \c TensKind of type TYPE */			\
  constexpr const int CONST_NAME=N;					\
  MAYBE_UNUSED(CONST_NAME);						\
									\
  DEFINE_TENS_COMP_CLASS(TYPE,N);					\
									\
  DEFINE_NAMED_BINDER(TYPE,BINDER)
}

#endif
