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
  /// Dynamic sizes of a Tens
  ///
  /// \todo the array must be replaced with a tuple, whose types
  /// must be deduced when instatiating the struct, such that int or
  /// long int or whatever is appropriately used!
  template <int N>
  using DynSizes=std::array<int,N>;
  
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
  /// calling it until one component is found. The check on types is
  /// done in the externally visible routine.
  template <int IDyn,     // Index of the current dynamic component in the list
	    class H,      // Current TensKind
	    class...Oth>  // Other TensKind
  struct _Indexer<IDyn,TensKind<H,Oth...>>
  {
    /// Size of the top-level class
    static constexpr int headSize=H::size;
    /// Check if this component is dynamic
    static constexpr bool thisDynamic=(headSize==DYNAMIC);
    /// Nested TensKind
    using NestedTk=TensKind<Oth...>;
    /// Nested Dynamic index
    static constexpr int nestedIDyn=(thisDynamic ? IDyn+1 : IDyn);
    /// Nested indexer
    using Nested=_Indexer<nestedIDyn,NestedTk>;
    
    /// Compute the index, given a set of components
    ///
    /// Internal implementation
    /// \todo fix the int types
    template <size_t NTotDyn,                                // Total number of dynamic components
	      class Head,                                    // Current component type
	      class...Tail,                                  // Other component types
	      class=ConstrainAreIntegrals<Head,Tail...>>     // Constrain all types to be integral
    static constexpr int index(const DynSizes<NTotDyn>& dynSizes,  ///< Dynamic sizes
			       const Head& head,                   ///< Current component
			       const Tail&...tail)                 ///< Other components
    {
      // Constrain the components to be in the same number of the type
      static_assert(sizeof...(Oth)==sizeof...(Tail),"Number of TensComp does not match number of passed components");
      // Nested value
      const int nested=Nested::index(forw<const DynSizes<NTotDyn>>(dynSizes),forw<const Tail>(tail)...);
      // Current component
      const int thisComp=head;
      //std::cout<<"Nested: "<<nested<<" , nested_head_size: "<<Nested::headSize<<" , thiscomp: "<<thisComp<<" , thissize: "<<headSize<<std::endl;
      // Nested size
      const int nestedSize=
	Nested::thisDynamic?
	dynSizes[nestedIDyn]:
	Nested::headSize;
#ifdef DEBUG_INDEXER
	  printf("NestedDyn: %d",Nested::thisDynamic);
	  printf(", Nested::HeadSize: %d",Nested::headSize);
	  printf(", %d",nestedIDyn);
	  if(Nested::thisDynamic) printf(" %d",dynSizes[nestedIDyn]);
	  printf("\n");
#endif
      
      // Compose nested value, size and this component
      return nested+nestedSize*thisComp;
    }
  };
  
  /// Indexer class to compute an index for a TensKind
  ///
  /// Single component case
  template <int IDyn,
	    class Head>
  struct _Indexer<IDyn,TensKind<Head>>
  {
    /// Size of the top-level class
    static constexpr int headSize=Head::size;
    /// Check if this component is dynamic
    static constexpr bool thisDynamic=(headSize==DYNAMIC);
    
    /// Compute the index, given a set of components
    /// \todo add check
    template <size_t NTotDyn>
    static int index(const DynSizes<NTotDyn>& dynSizes,
		     const int& head)
    {
      //std::cout<<"Non-Nested , thiscomp: "<<head<<" , thissize: "<<headSize<<std::endl;
      return head;
    }
  };
  
  /// Index function for a TensKind
  ///
  /// Wraps the call to the Indexer class method
  template <class TK,
	    size_t NDynamic,
	    class...Args>
  static constexpr int index(const DynSizes<NDynamic>& dynSizes, ///< Sizes of the dynamical components
			     const Args&...args)                 ///< Components index
  {
    static_assert(TK::nDynamic==NDynamic,"Nuber of dynamic components sizes must agree with the TensKind one");
    return _Indexer<0,TK>::index(dynSizes,args...);
  }
}

#endif
