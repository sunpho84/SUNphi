#ifndef _TENSSTOR_HPP
#define _TENSSTOR_HPP

/// \file TensStor.hpp
///
/// \brief Header file for the definition of a storage space for a tensor

#include <metaprogramming/SwallowSemicolon.hpp>
#include <system/Memory.hpp>
#include <tens/Indexer.hpp>
#include <tens/TensKind.hpp>

#include <cstdio>

namespace SUNphi
{
  /// Tensor Storage holding the resources for a tensor
  ///
  /// The tensor storage allocates and deallocates the memory location
  /// where a tensor is materially stored, keeping track of the amount
  /// of memory allocated. Facilities to reallocate the memory are
  /// provided
  template <class TK,class T>
  class TensStor :
    public ConstrainIsTensKind<TK> // Check that TK is a TensKind
  {
    /// Tuple containg all mapped type
    using type=typename TK::types;
    
    /// Internal storage
    T* v;
    
  public:
    
    /// Debug access to internal storage
    T* &_v=v;
    
    /// Debug store size
    int size;
    
    /// Defines a const or non-const evaluator
#define DEFINE_EVAL(CONST_TAG)							\
    /*! Returns a CONST_TAG reference to a TensStor given a set of components */ \
    template <class...Args,                          /* Arguments type */ \
	      class=ConstrainAreSame<int,Args...>>  /* Constrain all args to be integer */ \
    friend CONST_TAG T& eval(CONST_TAG TensStor& ts, /*!< Reference to the TensStor */ \
			     const Args&...args)     /*!< Components to extract */ \
    {									\
      const int id=index<TK>(ts.dynSizes,forw<const Args>(args)...); \
      /*printf("Index: %d\n",id);*/ /*debug*/				\
      									\
      return ts.v[id];							\
    }									\
    SWALLOW_SEMICOLON
    
    // Defines the non-const evaluator
    DEFINE_EVAL();
    
    // Defines the const evaluator
    DEFINE_EVAL(const);
    
    // Undefine the macro
#undef DEFINE_EVAL
    
    /// Dynamic sizes 
    std::array<int,TK::nDynamic> dynSizes;
    
    /// \todo the array must be replaced with a tuple, whose types
    /// must be deduced when instatiating the struct, such that int or
    /// long int or whatever is appropriately used!
    
    /// Constructor (test)
    template <class...DynSizes,                                  // Arguments (sizes)
	      class=ConstrainNTypes<TK::nDynamic,DynSizes...>>
    TensStor(const DynSizes&...extDynSizes) : dynSizes({{extDynSizes...}})
    {
      // Constrain the arguments to be all integer-like
      STATIC_ASSERT_ARE_INTEGRALS(DynSizes...);
      //printf("Ah ah! %d\n",TK::nDynamic);
      
      // Compute the size
      size=TK::maxStaticIdx;
      for(const auto &i : dynSizes)
	size*=i;
      
      // Allocate
      v=getRawAlignedMem<T>(size);
    }
    
    /// Destructor
    ~TensStor()
    {
      // Free
      freeMem(v);
    }
  };
  
}

#endif
