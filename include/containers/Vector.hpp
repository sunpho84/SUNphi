#ifndef _VECTOR_HPP
#define _VECTOR_HPP

/// \file Vector.hpp
///
/// \brief Header file for the definition of vectors, and useful routines related

#include <iostream>
#include <sstream>

#include <functional>
#include <numeric>
#include <map>
#include <vector>

#include <math/Arithmetic.hpp>
#include <metaprogramming/TypeTraits.hpp>
#include <metaprogramming/SFINAE.hpp>
#include <metaprogramming/UniversalReferences.hpp>
#include <utility/ScopeDoer.hpp>

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
	std::vector<T>::size();
      
      /// Converts to sign
      return
	static_cast<Size>(unsignedSize);
    };
    
    /// Copy constructor
    Vector(const Vector& oth) :
      std::vector<T>(oth.begin(),oth.end())
    {
    }
    
    /// Forward constructor to std::vector
    template <typename Arg,
	      typename...Args,
	      SFINAE_ON_TEMPLATE_ARG(not isSame<const Vector&,Arg>)>
    Vector(Arg&& arg,
	   Args&&...args) :
      std::vector<T>(forw<Arg>(arg),
		     forw<Args>(args)...)
    {
    }
    
    /// Default constructor
    Vector()
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
	  pos+=
	    off;
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
	loopUntil<true>(0,size(),cond,getNullAction());
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
    
    /// Finds the first element different from \c val
    Size findFirstDiffFrom(const T& val)              ///< Element to find
      const
    {
      return
	findFirstWhereNot(getComparer(val));
    }
    
    /// Finds the last element \c val
    Size findLast(const T& val)              ///< Element to find
      const
    {
      return
	loopUntil<false>(size()-1,-1,getComparer(val),getNullAction());
    }
    
    /// Finds the last element different from \c val
    Size findLastDiffFrom(const T& val)              ///< Element to find
      const
    {
      return
	findLastWhereNot(getComparer(val));
    }
    
    /// Check if the value is contained
    bool isContained(const T& val)
      const
    {
      return
	findFirst(val)!=size();
    }
    
    /// Push back a value and pop it back when going out of scope
    [[ nodiscard ]] auto scopePushBack(const T& val)
    {
      this->push_back(val);
	
      return
	ScopeDoer([this](){this->pop_back();});
    }
    
    /// Gets a string of form {1,2,3...}
    std::string getStr()
      const
    {
      /// Generator of the string
      std::ostringstream os;
      
      // Open bracket
      os<<"{";
      
      // First element
      if(size())
	os<<this->front();
      
      // Other elements
      for(Size iEl=1;iEl<size();iEl++)
	os<<","<<(*this)[iEl];
      
      // Close bracket
      os<<"}";
      
      return
	os.str();
    }
    
    /// Group the vector returning a map
    ///
    /// Example
    /// \code
    ///
    /// Vector v({2,2,3});
    /// v.group(); // {{2,2},{3,1}}
    /// \endcode
    std::map<T,T> group()
      const
    {
      /// Result containing grouped factors
      std::map<T,T> res;
      
      for(auto& f : *this)
	res[f]++;
      
      return
	res;
    }
    
    /// Returns the result and remainder of the division
    template <typename TOut>
    void divWithMod(Vector<TOut>& quotient,   ///< Result of the division
		    Vector<TOut>& remainder,  ///< Remainder of the division
		    const Vector& divisor)    ///< Divisor to be used
      const
    {
      for(Size i=0;
	  i<this->size();
	  i++)
	{
	  quotient[i]=
	    (*this)[i]/divisor[i];
	  
	  remainder[i]=
	    (*this)[i]%divisor[i];
	}
    }
  };
  
  /// Gets first or second entry of the map
  template <bool FirstSecond,
	    typename T1,
	    typename T2,
	    typename TOut=Conditional<FirstSecond,T2,T1>>
  Vector<TOut> getAllFirstOrSecond(const std::map<T1,T2>& m)
  {
    /// Result
    Vector<TOut> out;
    
    out.reserve(m.size());
    
    for(auto& it : m)
      if constexpr(FirstSecond==true)
	out.push_back(it.second);
      else
	out.push_back(it.first);
    
    return
      out;
  }
  
  /// Slice all keys of a map
  template <typename T1,
	    typename T2>
  auto getAllKeys(const std::map<T1,T2>& m)
  {
    return
      getAllFirstOrSecond<false>(m);
  }
  
  /// Slice all values of a map
  template <typename T1,
	    typename T2>
  auto getAllVal(const std::map<T1,T2>& m)
  {
    return
      getAllFirstOrSecond<true>(m);
  }
}

#endif
