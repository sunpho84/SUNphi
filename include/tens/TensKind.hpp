#ifndef _TENSKIND_HPP
#define _TENSKIND_HPP

/// \file TensKind.hpp
///
/// \brief Defines the Tensor Kind
///
/// The tensor kind defines the list of components that form a tensor

#include <metaprogramming/IntSeq.hpp>
#include <metaprogramming/Tuple.hpp>
#include <metaprogramming/TypeTraits.hpp>
#include <tens/TensComp.hpp>

namespace SUNphi
{
  /// Defines the BaseTensKind type traits
  ///
  DEFINE_BASE_TYPE(TensKind);
  
  /// Tensor Kind used to define the structure of a tensor
  ///
  /// The tensor kind defines the list of components of a tensor. It
  /// is used to the define the underlying set of components of a \c
  /// TensorStorage, or the returned type of a Template Expression
  ///
  template <class...T>
  class TensKind
  {
    //Check that all types are TensComp
    static_assert(IntSeq<IsTensComp<T>...>::hSum==sizeof...(T),"Cannot define a TensKind for types not inheriting from TensComp");
    
    typedef IntSeq<(T::size==DYNAMIC)...> IsDynamic; ///< An integer sequence defining whether the tuypes are dynamic or not
    
  public:
    
    static constexpr int nDynamic=
      IsDynamic::hSum;       ///< Number of dynamical components
    
    //Check that no dynamic type has been asked (temporarily)
    static_assert(nDynamic==0,"Not yet implemented the dynamic case");
    
    typedef Tuple<T...> type; ///< Tuple containing all types
    
    static constexpr int maxStaticIdx=
      IntSeq<(T::size>=0 ? T::size : 1)...>::hMul; ///< Maximal value of the index, restricted to the statical components
  };
}

#endif
