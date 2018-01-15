#ifndef _TENSCOMP_HPP
#define _TENSCOMP_HPP

/// \file TensComp.hpp
///
/// \brief Defines the Tensor Components

#include <metaprogramming/CRTP.hpp>
#include <metaprogramming/TypeTraits.hpp>
#include <system/SIMD.hpp>
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
  template <class T,            // Type defining the TensComp
	    int Size=DYNAMIC>   // Size of the TensComp
  struct TensComp :
    public BaseTensComp
  {
    /// Mapped type
    using type=T;
    
    // Cast to type
    PROVIDE_CRTP_CAST_OPERATOR(type);
    
    /// Size of the tensor component (max index value)
    static constexpr int size=Size;
    
    /// Report if the component has dynamic size
    static constexpr bool isDynamic=(Size==DYNAMIC);
    
    /// Check if compatible with vectorization
    template <typename F>                      // Fundamental type
    static constexpr bool isVectorizable=canBeFactorOfSIMDVector<F>(Size);
    
  };
  
  /// Defines a \c TensComp, with name TYPE and max N
#define DEFINE_TENS_COMP_CLASS(TYPE,N)					\
  /*! Tensor component of \c TYPE Kind */				\
  struct TYPE : public TensComp<TYPE,N>					\
  {									\
    PROVIDE_NAME(#TYPE);						\
  }
  
  /// Define a \c TensComp named \c TYPE with maximal component \c N
  ///
  /// Defines also a binder-instantiator named BINDER, a \c TensKind
  /// suffixing TYPE with Kind, and a constat holding the maximal value,
  /// named \c NCONST_NAME
#define DEFINE_TENS_COMP(BINDER,TYPE,CONST_NAME,HMANY)			\
									\
  /*! Maximal value for \c TensKind of type TYPE */			\
  constexpr const int N ## CONST_NAME=HMANY;				\
  MAYBE_UNUSED(N ## CONST_NAME);					\
									\
  DEFINE_TENS_COMP_CLASS(TYPE,HMANY);					\
									\
  DEFINE_NAMED_BINDER(TYPE,BINDER)
  
  /// Define row and \c TensComp named \c Rw/CnTYPE with maximal component \c N
  ///
  /// Defines also a common binder-instantiator named BINDER, a \c
  /// TensKind suffixing TYPE with Kind, and a constat holding the
  /// maximal value, named \c NCONST_NAME
#define DEFINE_RW_CN_TENS_COMP(BINDER,TYPE,CONST_NAME,HMANY)		\
									\
  /*! Number of components of TYPE type */				\
  constexpr const int N ## CONST_NAME=HMANY;				\
  MAYBE_UNUSED(N ## CONST_NAME);					\
									\
  /* Defines the Row TYPE component */					\
  DEFINE_TENS_COMP(rw ## TYPE,Rw ## TYPE,RW_ ## TYPE,N ## CONST_NAME);	\
									\
  /* Defines the Column TYPE component */				\
  DEFINE_TENS_COMP(cn ## TYPE,Cn ## TYPE,CN_ ## TYPE,N ## CONST_NAME);	\
									\
  /* Makes the Row and Column TYPE component twinned */			\
  DECLARE_TENS_COMPS_ARE_TWIN(Rw ## TYPE,Cn ## TYPE);			\
									\
  /* Declares a row or column (aliasing) binder for type TYPE */	\
  DEFINE_NAMED_RW_OR_COL_BINDER(TYPE,BINDER);				\
									\
  /*! Declares an alias TYPE to be used when no column or row specified */ \
  using TYPE=Cn ## TYPE
}

#endif
