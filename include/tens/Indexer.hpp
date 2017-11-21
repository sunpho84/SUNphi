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
    ///
    template <class T>
    struct Indexer : ConstraintIsTensKind<T>
    {
    };
    
    /// Indexer class to compute an index for a TensKind
    ///
    /// Recursive implementation definining a nested indexer and
    /// calling it until one component is found
    ///
    template <class H,class...Oth>
    struct Indexer<TensKind<H,Oth...>>
    {
      /// Size of the top-level class
      static constexpr int headSize=H::size;
      static_assert(headSize!=DYNAMIC,"Not yet implemented");
      
      /// Compute the index, given a set of components
      template <class...Tail,class=ConstraintAreSame<int,Tail...>>
      static constexpr int index(const int &head,const Tail&...tail)
      {
	static_assert(sizeof...(Oth)==sizeof...(Tail),"Number of TensComp does not match number of passed components");
	//Nested indexer
	using Nested=Indexer<TensKind<Oth...>>;
	//Nested value
	const int nested=Nested::index(tail...);
	//Current component
	const int thisComp=head;
	//Compose nested value, size and this component
	return nested+Nested::headSize*thisComp;
      }
    };
    
    /// Indexer class to compute an index for a TensKind
    ///
    /// Single component case
    ///
    template <class H>
    struct Indexer<TensKind<H>>
    {
      /// Size of the top-level class
      static constexpr int headSize=H::size;
      static_assert(headSize!=DYNAMIC,"Not yet implemented");
      
      /// Compute the index, given a set of components
      static constexpr int index(const int &head)
      {
	return head;
      }
    };
  }
  
  /// Index function for a TensKind
  ///
  /// Wraps the call to the Indexer class method
  ///
  template <class TK,class...Args>
  static constexpr int index(const Args&...args)
  {
    return Impl::Indexer<TK>::index(args...);
  }
}

#endif
