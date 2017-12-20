#ifndef _INDEXER_HPP
#define _INDEXER_HPP

/// \file Indexer.hpp
///
/// \brief Header file for the definition of an indexer

#include <metaprogramming/TypeTraits.hpp>
#include <system/Memory.hpp>
#include <tens/TensKind.hpp>

namespace SUNphi
{
  namespace Impl
  {
    /// Indexer class to compute an index for a TensKind
    ///
    /// Forward declaration, forbids instantiation
    template <int IDyn,
	      class T>
    struct _Indexer : ConstraintIsTensKind<T>
    {
    };
    
    /// Indexer class to compute an index for a TensKind
    ///
    /// Recursive implementation definining a nested indexer and
    /// calling it until one component is found
    template <int IDyn,
	      class H,
	      class...Oth>
    struct _Indexer<IDyn,TensKind<H,Oth...>>
    {
      // /// Index of the next dynamic component
      //static constexpr int iDyn=IDyn;
      /// Size of the top-level class
      static constexpr int headSize=H::size;
      /// Check if this component is dynamic
      static constexpr bool thisDynamic=(headSize==DYNAMIC);
      /// Nested TensKind
      using NestedTk=TensKind<Oth...>;
      /// Nested Dynamic index
      static constexpr int nestedIDyn=(thisDynamic ? IDyn+1 : IDyn);
      // Nested indexer
      using Nested=_Indexer<nestedIDyn,NestedTk>;
      
      /// Compute the index, given a set of components
      ///
      /// \todo fix the int
      template <size_t NTotDyn,
		class Head,
		class...Tail,
		class=ConstraintAreIntegrals<Head,Tail...>>
      static constexpr int index(const std::array<int,NTotDyn>& dynSizes,
				 const Head& head,
				 const Tail&...tail)
      {
	static_assert(sizeof...(Oth)==sizeof...(Tail),"Number of TensComp does not match number of passed components");
	// Nested value
	const int nested=Nested::index(std::forward<const std::array<int,NTotDyn>>(dynSizes),std::forward<const Tail>(tail)...);
	// Current component
	const int thisComp=head;
	//std::cout<<"Nested: "<<nested<<" , nested_head_size: "<<Nested::headSize<<" , thiscomp: "<<thisComp<<" , thissize: "<<headSize<<std::endl;
	// Nested size
	const int nestedSize=
	  Nested::thisDynamic?
	  dynSizes[nestedIDyn]:
	  Nested::headSize;
	printf("NestedDyn: %d, Nested::HeadSize: %d, %d %d\n",Nested::thisDynamic,Nested::headSize,nestedIDyn,dynSizes[nestedIDyn]);
	
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
      static int index(const std::array<int,NTotDyn>& dynSizes,
		       const int& head)
      {
	//std::cout<<"Non-Nested , thiscomp: "<<head<<" , thissize: "<<headSize<<std::endl;
	return head;
      }
    };
  }
  
  /// Index function for a TensKind
  ///
  /// Wraps the call to the Indexer class method
  template <class TK,
	    size_t NDynamic,
	    class...Args>
  static constexpr int index(const std::array<int,NDynamic>& dynSizes,
			     const Args&...args)
  {
    static_assert(TK::nDynamic==NDynamic,"Nuber of dynamci components sizes must agree with the TensKind one");
    return Impl::_Indexer<0,TK>::index(dynSizes,args...);
  }
}

#endif
