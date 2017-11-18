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
  
  /// \cond SUNphi_INTERNAL
  /// TensKind, forward definition
  ///
  template <class...T>
  class TensKind;
  /// \endcond
  
  /// Defines a TensKind type from a Tuple type
  ///
  DEFINE_VARIADIC_TYPE_FROM_TUPLE(TensKind);
  
  /// Tensor Kind used to define the structure of a tensor
  ///
  /// The tensor kind defines the list of components of a tensor. It
  /// is used to the define the underlying set of components of a \c
  /// TensorStorage, or the returned type of a Template Expression
  ///
  template <class...T>
  class TensKind : public BaseTensKind
  {
    //Check that all types are TensComp
    static_assert(IntSeq<IsTensComp<T>...>::hSum==sizeof...(T),"Cannot define a TensKind for types not inheriting from TensComp");
    
    typedef IntSeq<(T::size==DYNAMIC)...> AreDynamic; ///< An integer sequence defining whether the tuypes are dynamic or not
    
  public:
    
    static constexpr int nDynamic=
      AreDynamic::hSum;       ///< Number of dynamical components
    
    //Check that no dynamic type has been asked (temporarily)
    static_assert(nDynamic==0,"Not yet implemented the dynamic case");
    
    typedef Tuple<T...> Types; ///< Tuple containing all types
    
    template <class Tab>
    using AllButType=TensKindFromTuple<decltype(getAllBut<Tab>(Types{}))>; ///< Get all types but one
    
    static constexpr int maxStaticIdx=
      IntSeq<(T::size>=0 ? T::size : 1)...>::hMul; ///< Maximal value of the index, restricted to the statical components
  };
}

#endif
