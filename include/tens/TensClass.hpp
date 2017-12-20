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
  /// A Tensor class
  ///
  /// Container with a given TensKind structure and fundamental type,
  /// holding resources for the storage of the data and providing
  /// evaluator
  template <typename TK,   // List of tensor components
	    typename Fund> // Fundamental type
  class Tens :
    public ConstraintIsTensKind<TK>,        // Constraint the TK type to be a TensKind
    public ConstraintIsFloatingPoint<Fund>  // Constraint the Fund type to be a floating point
  {
    
  public:
    
    /// Tensor kind of the tensor
    using Tk=TK;
    
    std::array<int,Tk::nDynamic> &dynSizes=v->dynSizes;
    
    template <class...DynSizes>
    Tens(const DynSizes...extDynSizes) : v(new TensStor<Tk,Fund>(extDynSizes...))
    {
    }
    
    ~Tens()
    {
      delete v;
    }
    
  private:
    
    /// internal storage
    TensStor<Tk,Fund>* v=nullptr;
    
  public:
    
    /// Returns a constant reference to v
    const TensStor<Tk,Fund>& getStor() const
    {
      return *v;
    }
    
    /// Returns a non-constant reference to v
    TensStor<Tk,Fund>& getStor()
    {
      return *v;
    }
    
    /// Keep track that the class is storing data
    static constexpr bool isStoring=true;
    
    /// Evaluate the tensor (returns the index of the internal data)
    template <class...Comps,                               // Component types
	      class=ConstraintAreIntegrals<Comps...>,      // Force the components to be integer-like
	      class=ConstraintNTypes<Tk::nTypes,Comps...>> // Constraint the component to be in the same number of Tk
    friend double& eval(Tens& t,const Comps&...comps) // Component values
    {
      //print(cout,"Components: ",comps...,"\n");
      return eval(*t.v,std::forward<const Comps>(comps)...);
    }
  };
}

#endif
