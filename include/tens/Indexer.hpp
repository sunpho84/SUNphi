#ifndef _INDEXER_HPP
#define _INDEXER_HPP

/// \file Indexer.hpp
///
/// \brief Header file for the definition of an indexer

#ifdef HAVE_CONFIG_H
 #include <config.hpp>
#endif

#include <metaprogramming/TypeTraits.hpp>
#include <system/Memory.hpp>
#include <tens/TensKind.hpp>

namespace SUNphi
{
  /// Indexer class to compute an index for a TensKind
  ///
  /// Forward declaration
  template <int IDyn,          // Index of the current dynamic component
	    class T,           // Generic type
	    class=FalseType>   // Forbids instantiation
  struct _Indexer : ConstrainIsTensKind<T>
  {
  };
  
  /// Indexer class to compute an index for a TensKind
  ///
  /// Recursive implementation definining a nested indexer and
  /// calling it until no component is found. The check on types is
  /// done in the externally visible routine.
  template <int IDyn,     // Index of the current dynamic component in the list
	    class H,      // Current TensKind
	    class...Oth>  // Other TensKind
  struct _Indexer<IDyn,TensKind<H,Oth...>>
  {
    /// Size of the top-level class
    static constexpr int headSize=
      H::size;
    
    /// Check if this component is dynamic
    static constexpr bool thisDynamic=
      (headSize==DYNAMIC);
    
    /// Nested TensKind
    using NestedTk=
      TensKind<Oth...>;
    
    /// Nested Dynamic index
    static constexpr int nestedIDyn=
      (thisDynamic ? IDyn+1 : IDyn);
    
    /// Nested indexer
    using Nested=
      _Indexer<nestedIDyn,NestedTk>;
    
    /// Compute the index, given a set of components
    ///
    /// Internal implementation
    /// \todo fix the int types
    template <size_t NTotDyn,                                // Total number of dynamic components
	      class Head,                                    // Current component type
	      class...Tail,                                  // Other component types
	      class=ConstrainAreIntegrals<Head,Tail...>>     // Constrain all types to be integral
    static constexpr int index(const DynSizes<NTotDyn>& dynSizes,  ///< Dynamic sizes
			       const int in,                       ///< External partial index
			       const Head& head,                   ///< Current component
			       const Tail&...tail)                 ///< Other components
    {
      // Constrain the components to be in the same number of the type
      static_assert(sizeof...(Oth)==sizeof...(Tail),"Number of TensComp does not match number of passed components");
      
      // Current component
      const int thisComp=
	head;
      
      // Current size
      const int size=
	thisDynamic?
	dynSizes[IDyn]:
	headSize;
      
      // Compute the result
      int out=thisComp+size*in;
	  
      // Nested value
      if constexpr(sizeof...(Tail)>0)
	{
#ifdef DEBUG_INDEXER
	  printf("Is Nested , thiscomp: %d , is Dyn: %d",thisComp,thisDynamic);
	  printf(", HeadSize: %d",headSize);
	  printf(", Size: %d",size);
	  printf(", %d",IDyn);
	  if(thisDynamic) printf(" %d",dynSizes[IDyn]);
	  printf("\n");
#endif
	  
	  return Nested::index(forw<const DynSizes<NTotDyn>>(dynSizes),out,forw<const Tail>(tail)...);
	}
      else
	{
#ifdef DEBUG_INDEXER
	  std::cout<<"Non-Nested , value: "<<out<<std::endl;
#endif
	  return out;
	}
    }
  };
  
  /// Index function for a TensKind
  ///
  /// Wraps the call to the Indexer class method
  /// The index is built in this way.
  /// Let us assume three component case
  ///
  /// \code
  /// a+Na*(b+Nb*c)
  /// \endcode
  ///
  /// this is changed into a more homogeneous
  ///
  /// \code
  /// a+Na*(b+Nb*(c+Nc*0))
  /// \endcode
  ///
  /// so that every component needs to perform the following operation
  ///
  /// \code
  /// i+N*ext
  /// \endcode
  ///
  /// where \c ext is the result of the calculation for the outer
  /// component. Starting from 0, the calculation ends when no more
  /// component is present (terminating case).
  template <class TK,         // Tensor Kind
	    size_t NDynamic,  // Number of dynamic components
	    class...Args>     // Types of the components
  static constexpr int index(const DynSizes<NDynamic>& dynSizes, ///< Sizes of the dynamical components
			     const Args&...args)                 ///< Components index
  {
    static_assert(TK::nDynamic==NDynamic,"Nuber of dynamic components sizes must agree with the TensKind one");
    STATIC_ASSERT_ARE_INTEGRALS(Args...);
    
#ifdef DEBUG_INDEXER
    print(std::cout,"Components:",args...,"\n");
#endif
    
    return _Indexer<0,TK>::index(dynSizes,0,args...);
  }
}

#endif
