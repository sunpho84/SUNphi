#ifndef _TENSCLASS_HPP
#define _TENSCLASS_HPP

/// \file TensClass.hpp
///
/// \brief Header file for the Tens class

#include <metaprogramming/IntSeq.hpp>
#include <tens/TensKind.hpp>
#include <tens/TensStor.hpp>
#include <tex/UnaryTEx.hpp>

namespace SUNphi
{
  // Base type to qualify as Tens
  DEFINE_BASE_TYPE(Tens);
  
  /// A Tensor class
  ///
  /// Container with a given TensKind structure and fundamental type,
  /// holding resources for the storage of the data and providing
  /// evaluator
  template <typename TK,   // List of tensor components
	    typename Fund> // Fundamental type
  class Tens :
    public BaseTens,                       // Inherit from BaseTens to detect in expression
    public UnaryTEx<Tens<TK,Fund>>,        // Inherit from UnaryTEx
    public ConstrainIsTensKind<TK>,        // Constrain the TK type to be a TensKind
    public ConstrainIsFloatingPoint<Fund>  // Constrain the Fund type to be a floating point
  {
    
  public:
    
    /// Tensor kind of the tensor
    using Tk=TK;
    
    // Attributes
    ASSIGNABLE;
    STORING;
    IS_ALIASING_ACCORDING_TO_POINTER(v);
    
  private:
    
    /// Internal storage
    TensStor<Tk,Fund>* v=nullptr;
    
  public:
    
    /// Sizes of the dynamical components, reference to the storage
    DynSizes<Tk::nDynamic> &dynSizes;
    
    /// Construct the Tens on the basis of the dynamical sizes passed
    template <class...DynSizes>                    //   Dynamic size types
    Tens(const DynSizes&...extDynSizes) :          ///< Passed internal dynamic size
      v(new TensStor<Tk,Fund>(extDynSizes...)),    //   Construct the vector before taking the reference
      dynSizes(v->dynSizes)                        //   Assign the storage dynamical size reference
    {
      STATIC_ASSERT_ARE_N_TYPES(Tk::nDynamic,DynSizes);
    }
    
    /// Destructor
    ~Tens()
    {
      delete v;
    }
    
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
    
    /// Evaluate the tensor (returns the index of the internal data)
    /// \todo Enable only for Tens TEx
    template <class T,
	      class...Comps,                              // Component types
	      class=ConstrainAreIntegrals<Comps...>,      // Force the components to be integer-like
	      class=ConstrainNTypes<Tk::nTypes,Comps...>> // Constrain the component to be in the same number of Tk
    friend DECLAUTO eval(T&& t,                ///< Tensor to evaluate
			 const Comps&...comps) ///< Component values
    {
      //print(cout,"Components: ",comps...,"\n");
      return eval(*t.v,forw<const Comps>(comps)...);
    }
  };
  
  // Check that a test Tens is a UnaryTEx
  STATIC_ASSERT_IS_TEX(Tens<TensKind<TensComp<double,1>>,double>);
}

#endif
