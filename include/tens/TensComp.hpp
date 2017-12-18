#ifndef _TENSCOMP_HPP
#define _TENSCOMP_HPP

/// \file TensComp.hpp
///
/// \brief Defines the Tensor Components

#include <metaprogramming/TypeTraits.hpp>

namespace SUNphi
{
  /// Defines the BaseTensCompKind type traits
  ///
  /// Used to specify the underlying kind of a tensor component
  DEFINE_BASE_TYPE(TensCompKind);
  
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
    public BaseTensComp,ConstraintIsTensCompKind<T>
  {
    /// Mapped type
    using type=T;
    
    /// Size of the tensor component (max index value)
    static constexpr int size=Size;
  };
}

/// Defines a \c TensComp, with name TYPE and max N
#define DEFINE_NAMED_TENS_COMP(TYPE,N)					\
  /*! Tensor component of \c TYPE Kind */				\
  struct TYPE : public TensComp<TYPE ## Kind,N>				\
  {									\
    /*! Returns the name of the type, as a std string. TO BE IMPROVED */ \
    static std::string name()						\
    {									\
      return #TYPE;							\
    }									\
  }

#endif
