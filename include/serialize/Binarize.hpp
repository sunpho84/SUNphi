#ifndef _BINARIZE_HPP
#define _BINARIZE_HPP

/// \file Binarize.hpp
///
/// \brief Converts to binary

#include <string>

#include <debug/Crash.hpp>
#include <ios/Logger.hpp>
#include <metaprogramming/SFINAE.hpp>

namespace SUNphi
{
  /// Class to convert objects to binary
  class Binarizer
  {
    /// Buffer
    std::vector<char> buf;
    
    /// Push data on the back
    Binarizer& pushBack(const void* data,   ///< Data to be pushed
			const size_t& size) ///< Data size
    {
      buf.insert(buf.end(),(char*)data,(char*)data+size);
      
      return
	*this;
    }
    
  public:
    
    /// Used size
    size_t size()
      const
    {
      return
	buf.size();
    }
    
    /// Add both begin and end method, with and not const
#define PROVIDE_BEGIN_END(CV)				\
    /*! Pointer to the beginning of the buffer */	\
    CV auto begin()					\
      CV						\
    {							\
      return						\
	buf.begin();					\
    }							\
							\
    /*! Pointer to the end of the buffer */		\
    CV auto end()					\
      CV						\
    {							\
      return						\
	buf.end();					\
    }							\
    SWALLOW_SEMICOLON_AT_CLASS_SCOPE
    
    PROVIDE_BEGIN_END(/* */);
    PROVIDE_BEGIN_END(const);
    
#undef PROVIDE_BEGIN_END
    
    /// Write on the binarizer
    template <typename T,
	      SFINAE_ON_TEMPLATE_ARG(std::is_trivially_copyable_v<T>)>
    Binarizer& operator<<(const T& rhs)
    {
      return
	pushBack(&rhs,sizeof(T));
    }
    
    /// Binarize a string
    Binarizer& operator<<(const std::string& in)     ///< Input
    {
      (*this)<<
	in.size();
      
      return
	this->pushBack(&in[0],in.size());
    }
  };
}

#endif
