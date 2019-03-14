#ifndef _BINARIZE_HPP
#define _BINARIZE_HPP

/// \file Binarize.hpp
///
/// \brief Converts to and from binary

#include <string>

#include <debug/Crash.hpp>
#include <ios/Logger.hpp>
#include <metaprogramming/CRTP.hpp>
#include <metaprogramming/SFINAE.hpp>

namespace SUNphi
{
  /// Class to convert objects to and from binary
  class Binarizer
  {
    /// Buffer
    std::vector<char> buf;
    
    /// Reading position
    size_t readPos{0};
    
    /// Push data on the back
    Binarizer& pushBack(const void* data,   ///< Data to be pushed
			const size_t& size) ///< Data size
    {
      buf.insert(buf.end(),(char*)data,(char*)data+size);
      
      return
	*this;
    }
    
    /// Push data on the back
    Binarizer& readAdvancing(void* out,          ///< Data to be filled
			     const size_t& size) ///< Data size
    {
      memcpy(out,&buf[readPos],size);
      
      readPos+=
	size;
      
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
    
    /// Read from the binarizer
    template <typename T,
	      SFINAE_ON_TEMPLATE_ARG(std::is_trivially_copyable_v<T>)>
    Binarizer& operator>>(T& rhs)
    {
      return
	readAdvancing(&rhs,sizeof(T));
    }
    
    /// Binarize a string
    Binarizer& operator<<(const std::string& in)     ///< Input
    {
      /// It is important to store the terminator
      const size_t nullTerminatedSize=
	in.size()+1;
      
      (*this)<<
	nullTerminatedSize;
      
      return
	this->pushBack(&in[0],nullTerminatedSize);
    }
    
    /// DeBinarize a string
    Binarizer& operator>>(std::string& rhs)     ///< Output
    {
      /// Null terminated string
      size_t nullTerminatedSize;
      
      (*this)>>
	nullTerminatedSize;
      
      rhs.resize(nullTerminatedSize-1);
      
      return
	readAdvancing(&rhs[0],nullTerminatedSize);
    }
    
    /// Restart from head
    void restartReading()
    {
      readPos=
	0;
    }
    
    /// Restart to write
    void clear()
    {
      buf.clear();
      restartReading();
    }
  };
  
  /// Provides methods binarize through CRTP
  template <typename T>
  class Binarizable
  {
    PROVIDE_CRTP_CAST_OPERATOR(T);
    
  public:
    
    /// Creates the binarized version
    Binarizer binarize()
      const
    {
      /// Output binarizer
      Binarizer out;
      
      return
	out<<
	~(*this);
    }
    
    /// Fill from a binarized version
    void deBinarize(Binarizer& deb)
    {
      deb>>
	~(*this);
    }
  };
}

#endif
