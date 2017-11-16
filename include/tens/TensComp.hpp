#ifndef _TENSCOMP_HPP
#define _TENSCOMP_HPP

/// \file TensComp.hpp
///
/// \brief Defines the Tensor Components

#include <metaprogramming/TypeTraits.hpp>

namespace SUNphi
{
  /// Defines the BaseTensComp type traits
  ///
  DEFINE_BASE_TYPE(TensComp);
  
  /// Constant used to instantiate a Dynamic-size TensComp
  ///
  constexpr int DYNAMIC=-1;
  
  /// Tensor Component
  ///
  /// A tensor component is defined by an internal type and the
  /// associated size, which can be known at compile-time or can be
  /// made \c DYNAMIC, to be specified at runtime in the storage class
  ///
  template <class T,int Size=DYNAMIC>
  struct TensComp : public BaseTensComp
  {
    using type=T;                         ///< Mapped type
    static constexpr int size=Size;       ///< Size of the tensor component (max index value)
  };
}

#endif
