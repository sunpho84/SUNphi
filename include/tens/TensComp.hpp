#ifndef _TENSCOMP_HPP
#define _TENSCOMP_HPP

/// \file TensComp.hpp
///
/// \brief Defines the Tensor Components

#include <metaprogramming/TypeTraits.hpp>

namespace SUNphi
{
  DEFINE_BASE_TYPE(TensComp);
  
  /// Static version of the TensCompT
  ///
  /// A tensor component with a compile-time known value of max index
  ///
  template <class T,int Size>
  struct TensCompT : public BaseTensComp
  {
    static constexpr bool isDynamic=false; ///< Keep track of the fact that the component max value is not dynamic
    static constexpr int  size=Size;       ///< Size of the tensor component (max index value)
  };
  
  /// Constant used to instantiate a Dynamic-size TensCompT
  ///
  constexpr int DYNAMIC=-1;
  
  /// Dynamic version of the TensCompT
  ///
  /// A tensor component with a run-time known value of max index
  ///
  template <class T>
  struct TensCompT<T,DYNAMIC> : public BaseTensComp
  {
    static constexpr bool isDynamic=true; ///< Keep track of the fact that the component max value is dynamic
    const int &size;                       ///< Size of the tensor (reference to external size)
    
    TensCompT(const int &size) : size(size) {} ///< Default constructor taking the size as a constant reference
  };
  
  /// Counts the number of dynamic components of a list of tensor types
  ///
  /// Multiple parameter case, calling iteratively itself
  ///
  template <typename T,typename...comps>
  constexpr int NDynComps=NDynComps<T>+NDynComps<comps...>;
  
  /// Counts the number of dynamic components of a list of tensor types
  ///
  /// Terminator for the single type case
  ///
  template <class T>
  constexpr int NDynComps<T> =T::isDynamic;
  
  // template<class T>
  // using is_dynamic_comp_t=std::conditional_t<T::is_dynamic,std::true_type,std::false_type>;
  
  // template<class T>
  // using is_static_comp_t=std::conditional_t<not T::is_dynamic,std::true_type,std::false_type>;
}

#endif
