#ifndef _TENSCLASS_HPP
#define _TENSCLASS_HPP

/// \file TensClass.hpp
///
/// \brief Header file for the Tens class

#include <metaprogramming/IntSeq.hpp>
#include <tens/TensKind.hpp>
#include <tens/TensStor.hpp>

namespace SUNphi
{
  template <typename TK,   // List of tensor components
	    typename Fund> // Fundamental type
  class Tens :
    public ConstraintIsTensKind<TK>,        // Constraint the TK type to be a TensKind
    public ConstraintIsFloatingPoint<Fund>  // Constraint the Fund type to be a floating point
  {
    
  public:
    
    /// Tensor kind of the tensor
    using Tk=TK;
    
  private:
    
    /// internal storage
    TensStor<Tk,Fund> v;
    
  public:
    
    /// Returns a constant reference to v
    const TensStor<Tk,Fund>& get_v() const
    {
      return v;
    }
    
    /// Returns a non-constant reference to v
    TensStor<Tk,Fund>& get_v()
    {
      return v;
    }
    
    /// Keep track that the class is storing data
    static constexpr bool isStoring=true;
    
    /// Evaluate the tensor (returns the index of the internal data)
    template <class...Comps>                          // Component types: they must be integer
    friend double& eval(Tens& t,const Comps&...comps) // Component values
    {
      // Constraint the argument to be in the same number of the components
      static_assert(sizeof...(comps)==Tk::nTypes,"The number of arguments must be the same of the number of components");
      
      // Constraint the arguments to be all integer
      static_assert(IntSeq<IsIntegral<Comps>...>::hMul==1,"All components must be integer constants");
      
      //print(cout,"Components: ",comps...,"\n");
      return eval(t.v,std::forward<const Comps>(comps)...);
    }
  };
}

#endif
