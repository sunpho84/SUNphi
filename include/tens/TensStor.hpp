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
  /// provided.
  ///
  /// \todo many things, including considering a static storage if
  /// size is small and statically known
  template <class TK,
	    class T>
  class TensStor :
    public ConstrainIsTensKind<TK> // Check that TK is a TensKind
  {
    /// Tuple containg all mapped type
    using type=typename TK::types;
    
    /// Internal storage
    T* v;
    
    /// Store whether this has been created or is a reference
    bool created=
      false;
    
  public:
    
    /// Tensor Kind mapped
    using Tk=TK;
    
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
    
    /// Returns the total size of the range [Beg, End)
    template <int Beg, // Begin of the range
	      int End> // End of the range
    int compsRangeSize() const
    {
      // If empty range, returns 0
      if constexpr(Beg>=End)
	return 0;
      else
	{
	  // TensComp number Beg
	  using Tc=TupleElementType<Beg,typename TK::types>;
	  // Compute this comp size
	  int tmp=compSize<Tc>();
	  // If more components present, nest
	  if constexpr(Beg+1<End)
	    return tmp*compsRangeSize<Beg+1,End>();
	  // Otherwise return
	  else
	    return tmp;
	}
    }
    
    /// Creat the full list of dynamical sizes of the grouped comps
    template <int...Delims,   // Delimiters of the groups
    	      int...DynComps> // List of dynamical comps
    DECLAUTO compsRangeGroupsSize(const IntSeq<Delims...>&,
				  const IntSeq<DynComps...>& mergedDynCompPos) const
    {
      using Is=IntSeq<Delims...>;
      
      DynSizes<sizeof...(DynComps)> sizes=
	{{compsRangeSize<
	  Is::template element<DynComps>(),
	  Is::template element<DynComps+1>()>()...}};
      
      return sizes;
    }
    
    /// Returns a components-merged version of the TensStor
    template <typename Is>  // Integer sequence containing the delimiters
    DECLAUTO mergedComps() const
    {
      /// Returned TensKind
      using MergedTk=
	typename TK::template Merged<Is>;
      
      /// Returned type
      using TOut=
	TensStor<MergedTk,T>;
      
      /// Position of the merged dynamical components
      using MergedDynCompPos=
	typename MergedTk::DynCompsPos;
      
      /// Dynamic sizes after merge
      const DynSizes<MergedTk::nDynamic> mergedDynSizes=
	compsRangeGroupsSize(Is{},MergedDynCompPos{});
      
      return new TOut(mergedDynSizes,v);
    }
    
    /// Allocator
    void alloc()
    {
      // Mark down that we are creating
      created=
	true;
      
      // Compute the size
      totSize=
	TK::maxStaticIdx;
      for(const auto &i : dynSizes)
	totSize*=
	  i;
      
      // Allocate
      v=getRawAlignedMem<T>(totSize);
      
#ifdef DEBUG_STOR
      runLog()<<"TensStor constructor: "<<v<<", "<<__PRETTY_FUNCTION__;
#endif
    }
    
    /// Constructor (test)
    template <class...DynSizes,                                               // Arguments (sizes)
	      typename=EnableIf<areIntegrals<Unqualified<DynSizes>...>>,      // Constrain to be integers
	      class=ConstrainNTypes<TK::nDynamic,DynSizes...>>                // Constrain to be in the correct number
    explicit TensStor(DynSizes&&...extDynSizes) :
      dynSizes({{extDynSizes...}})          // Store the sizes
    {
      // Constrain the arguments to be all integer-like
      STATIC_ASSERT_ARE_INTEGRALS(Unqualified<DynSizes>...);
      //printf("Ah ah! %d\n",TK::nDynamic);
      
      alloc();
    }
    
    /// Constructor taking dynSizes and pointer (test)
    explicit TensStor(const DynSizes<TK::nDynamic>& dynSizes,
		      T* v) :
      v(v),                       // Copy the ref
      dynSizes(dynSizes)          // Store the sizes
    {
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
      runLog()<<"TensStor destructor: "<<v<<", "<<__PRETTY_FUNCTION__;
#endif
      
      // Free
      if(created)
	freeMem(v);
    }
  };
  
}

#endif
