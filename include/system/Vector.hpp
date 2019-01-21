#ifndef _VECTOR_HPP
#define _VECTOR_HPP

/// \file Vector.hpp
///
/// \brief Header file for the definition of vectors, and useful routines related

#include <iostream>

#include <functional>
#include <numeric>
#include <vector>

#include <metaprogramming/TypeTraits.hpp>
#include <metaprogramming/UniversalReferences.hpp>

namespace SUNphi
{
  /// Class wrapping std::vector
  ///
  /// Fix some some idiosinchrasy, such as the unsigned returned value
  /// of size, and adds some useful method
  template <typename T>         // Type of objects inside the vector
  class Vector :
    public std::vector<T>
  {
    /// Type of the data returned by std::vector
    using UnsignedSize=
      typename std::vector<T>::size_type;
    
  public:
    
    /// Signed version of the std::vector<T> size
    using Size=
      SignedOf<UnsignedSize>;
    
    /// Returns the signed size
    Size size() const
    {
      /// Gets the unsigned size
      const UnsignedSize unsignedSize=
	static_cast<const std::vector<T>>(*this).size();
      
      /// Converts to sign
      return
	static_cast<Size>(unsignedSize);
    };
    
    /// Forward constructor to std::vector
    template <typename...Args>
    Vector(Args&&...args) :
      std::vector<T>(forw<Args>(args)...)
    {
    }
    
    /// Forward constrcution by initializing list
    Vector(std::initializer_list<T> il) :
      std::vector<T>(il)
    {
    }
    
    /// Computes the sum of all elements between \c [beg:end)
    T summatorial(const UnsignedSize beg, ///< Begin
		  const UnsignedSize end) ///< End
      const
    {
      return
	std::accumulate(&(*this)[beg],&(*this)[end],T{0});
    }
    
    /// Computes the sum of all elements between \c [beg:end)
    ///
    /// Case in which one only or no extreme is provided
    T summatorial(const UnsignedSize beg=0) ///< Begin
      const
    {
      return
	summatorial(beg,size());
    }
    
    /// Computes the product of all elements between \c [beg:end)
    T productorial(const UnsignedSize beg, ///< Begin
		   const UnsignedSize end) ///< End
      const
    {
      return
	std::accumulate(&(*this)[beg],&(*this)[end],T{1},std::multiplies<T>());
    }
    
    /// Computes the product of all elements between \c [beg:end)
    ///
    /// Case in which one only or no extreme is provided
    T productorial(const UnsignedSize beg=0) ///< Begin
      const
    {
      return
	productorial(beg,size());
    }
    
    /// Null action
    static auto getNullAction()
    {
      return
	[](const Vector<T>& v,Size pos){};
    }
    
    /// True condition
    static auto getTrueCondition()
    {
      return
	[](const Vector<T>& v,Size pos){return true;};
    }
    
    /// Returns an operator acting on a given element
    template <typename F>
    static auto getActer(F f)
    {
      return
	[f](const Vector<T>& v,Size pos) mutable
	{
	  return
	    f(v,pos);
	};
    }
    
    /// Returns an operator comparing and returning Ret when occurring, !Ret if not
    template <bool Ret=true>                    // Returned value
    static auto getComparer(const T& val)     ///< Value to compare
    {
      return
	getActer([val](const Vector<T>& v,Size pos) mutable
		 {
		   if(v[pos]==val)
		     return
		       Ret;
		   else
		     return
		       not Ret;
		 });
    }
    
    /// Perform the action all elements starting from first up to last (excluded) or until the condition is true or false
    ///
    /// Returns the last position where the condition was true, or the
    /// past-beyond delimiter if never occurred
    template <bool falseTrue,  /// Go on until the condition is false or true
	      typename C,      /// Condition function type
	      typename A>      /// Action to perform
    Size loopUntil(const Size first,     ///< First element to loop on
		   const Size last,      ///< Last excluded element to loop on
		   C cond,               ///< Condition function
		   A act)                ///< Action to perform
      const
    {
      /// Offset to move
      const Size off=
	sign(last-first);
      
      /// Position
      Size pos=
	first;
      
      // Loop
      while(pos!=last and cond(*this,pos)==falseTrue)
	{
	  act(*this,pos);
	  pos+=off;
	}
      
      return
	pos;
    }
    
    /// Finds the first element where the condition is false
    template <typename C>
    Size findFirstWhereNot(C cond)                 ///< Condition to check
      const
    {
      return
	loopUntil<false>(0,size(),cond,getNullAction());
    }
    
    /// Finds the last element where the condition is false
    template <typename C>
    Size findLastWhereNot(C cond)                 ///< Condition to check
      const
    {
      return
	loopUntil<false>(size()-1,-1,cond,getNullAction());
    }
    
    /// Finds the first element \c val
    Size findFirst(const T& val)              ///< Element to find
      const
    {
      return
      	loopUntil<false>(0,size(),getComparer(val),getNullAction());
    }
    
    /// Finds the last element \c val
    Size findLast(const T& val)              ///< Element to find
      const
    {
      return
	loopUntil<false>(size()-1,-1,getComparer(val),getNullAction());
    }
    
  };
}

#endif
