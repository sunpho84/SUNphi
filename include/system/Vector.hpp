#ifndef _VECTOR_HPP
#define _VECTOR_HPP

/// \file Vector.hpp
///
/// \brief Header file for the definition of vectors, and useful routines related

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
    using SignedSize=
      SignedOf<UnsignedSize>;
    
    /// Returns the signed size
    SignedSize size() const
    {
      /// Gets the unsigned size
      const UnsignedSize unsignedSize=
	static_cast<const std::vector<T>>(*this).size();
      
      /// Converts to sign
      return
	static_cast<SignedSize>(unsignedSize);
    };
    
    /// Forward constructor to std::vector
    template <typename...Args>
    Vector(Args&&...args) :
      std::vector<T>{forw<Args>(args)...}
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
    {
      return
	productorial(beg,size());
    }
    
  };
}

#endif
