#ifndef _TENSSTOR_HPP
#define _TENSSTOR_HPP

/// \file TensStor.hpp
///
/// \brief Header file for the definition of a storage space for a tensor

#include <metaprogramming/SFINAE.hpp>
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
  template <class TK,
	    class T>
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
    int totSize;
    
    /// Defines a const or non-const evaluator
#define PROVIDE_EVAL(QUALIFIER)						\
    /*! Returns a QUALIFIER reference to a TensStor given a set of components            */ \
    template <class...Args,                          /* Arguments type                   */ \
	      class=ConstrainAreSame<int,Args...>>   /* Constrain all args to be integer */ \
    QUALIFIER T& eval(const Args&...args) QUALIFIER  /*!< Components to extract          */ \
    {									\
      const int id=index<TK>(dynSizes,forw<const Args>(args)...);	\
      /* printf("Index: %d\n",id);*/ /*debug*/				\
      									\
      return v[id];							\
    }									\
    SWALLOW_SEMICOLON_AT_CLASS_SCOPE
    
    PROVIDE_EVAL(NON_CONST_QUALIF);
    PROVIDE_EVAL(CONST_QUALIF);
    
    // Undefine the macro
#undef PROVIDE_EVAL
    
    /// Dynamic sizes
    DynSizes<TK::nDynamic> dynSizes;
    
    /// Returns the size of a given component in the case it is Dynamic
    template <typename TC,
	      SFINAE_ON_TEMPLATE_ARG(isDynamic<TC>)>
    constexpr int compSize() const
    {
      return dynSizes[TK::template dynCompPos<TC>];
    }
    
    /// Returns the size of a given component in the case it is not Dynamic
    template <typename TC,
	      SFINAE_ON_TEMPLATE_ARG(not isDynamic<TC>)>
    int compSize() const
    {
      return TC::size;
    }
    
    // /// returns a components-merged version
    // template <typename Is>
    // TensStor<typename TK::template Merged<Is>,T> merged() const
    // {
      
    //   TensStor<typename TK::template Merged<Is>,T> out(v)
    // }
    
    /// Allocator
    void alloc()
    {
      // Compute the size
      totSize=TK::maxStaticIdx;
      for(const auto &i : dynSizes)
	totSize*=i;
      
      // Allocate
      v=getRawAlignedMem<T>(totSize);
      
#ifdef DEBUG_STOR
      using namespace std;
      cout<<"TensStor constructor: "<<v<<", "<<__PRETTY_FUNCTION__<<endl;
#endif
    }
    
    /// Constructor (test)
    template <class...DynSizes,                                  // Arguments (sizes)
	      class=ConstrainNTypes<TK::nDynamic,DynSizes...>>
    explicit TensStor(const DynSizes&...extDynSizes) : dynSizes({{extDynSizes...}})
    {
      // Constrain the arguments to be all integer-like
      STATIC_ASSERT_ARE_INTEGRALS(DynSizes...);
      //printf("Ah ah! %d\n",TK::nDynamic);
      
      alloc();
    }
    
    /// Copy constructor (test)
    ///
    /// \todo improve memcpy, this could be parallelized in many ways,
    /// we have to implement memory manager soon
    explicit TensStor(const TensStor& oth) :
      dynSizes(oth.dynSizes)
    {
      alloc();
      
      for(int i=0;i<totSize;i++)
	v[i]=oth.v[i];
    }
    
    /// Destructor
    ~TensStor()
    {
#ifdef DEBUG_STOR
      using namespace std;
      cout<<"TensStor destructor: "<<v<<", "<<__PRETTY_FUNCTION__<<endl;
#endif
      
      // Free
      freeMem(v);
    }
  };
  
}

#endif
