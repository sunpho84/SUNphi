#ifndef _PARTITION_HPP
#define _PARTITION_HPP

/// \file Partition.hpp
///
/// \brief Factorizes numbers into partitions

#include <math/Factorize.hpp>
#include <utility/Combinatorial.hpp>

namespace SUNphi
{
  /////////////////////////////////////////////////////////////////
  
  namespace Impl
  {
    /// Loop on all factorizing partition of n
    ///
    /// Internal implementation, proceeds recursively calling the
    /// function until the position is the end
    template <typename I,
	      typename Fun>
    I loopOnAllFactorizingPartitioning(Vector<I>& partition,  ///< Progressively built partition
				       const I& n,            ///< Numbr to factorize
				       const Fun& fun,        ///< Function to be called
				       const int& pos=0)      ///< Position filled at this iteration
    {
      /// Number of partition found
      I nPart=
	0;
      
      if(pos+1<partition.size())
	loopOnAllSubmultiplesOf(n,[&partition,&n,&pos,&fun,&nPart](const I& factor)
				  {
				    partition[pos]=
				      factor;
				    
				    nPart+=
				      loopOnAllFactorizingPartitioning(partition,n/factor,fun,pos+1);
				  });
      else
	{
	  partition[pos]=
	    n;
	  
	  fun(partition);
	  
	  nPart=
	    1;
	}
      
      return
	nPart;
    }
  }
  
  /// Loop on all factorizing partition of n
  ///
  /// Returns the number of partitions obtained
  template <typename I,
	    typename Fun>
  I loopOnAllFactorizingPartitioning(const I& n,        ///< Number to factorize
				     const Fun& fun,    ///< Function to be invocated at each turn
				     const int& nFacts) ///< Size of the partition
  {
    /// Built partition
    Vector<I> partition(nFacts);
    
    return
      Impl::loopOnAllFactorizingPartitioning(partition,n,fun);
  }
  
  /// Gets the list of all factorizing partition of n
  template <typename I>
  Vector<Vector<I>> listAllFactorizingPartitioning(const I& n,        ///< Number to factorize
						   const int& nFacts) ///< Size of the partition
  {
    Vector<Vector<I>> list;
    
    loopOnAllFactorizingPartitioning(n,
				     [&list](const Vector<I>& partition)
				     {
				       list.push_back(partition);
				     },
				     nFacts);
    
    return
      list;
  }
}

#endif
