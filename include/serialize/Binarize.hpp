#ifndef _BINARIZE_HPP
#define _BINARIZE_HPP

/// \file Binarize.hpp
///
/// \brief Converts to and from binary

#include <cstring>
#include <string>
#include <vector>

#include <metaprogramming/CRTP.hpp>
#include <metaprogramming/SFINAE.hpp>

#include <ios/MinimalLogger.hpp>

namespace SUNphi
{
  DEFINE_HAS_MEMBER(binarize);
  DEFINE_HAS_MEMBER(deBinarize);
  
  /// Determine whether a type is serializable
  template <typename T>
  [[ maybe_unused ]]
  constexpr bool isBinarizable=
    hasMember_binarize<T> and hasMember_deBinarize<T>;
  
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
    
    /// Write on the binarizer, if the type has a member binarize
    template <typename T,
	      SFINAE_ON_TEMPLATE_ARG(hasMember_binarize<T>)>
    Binarizer& binarize(const T& rhs)
    {
      return
	rhs.binarize(*this);
    }
    
    /// Read from the binarizer, if the type has a member deBinarize
    template <typename T,
	      SFINAE_ON_TEMPLATE_ARG(hasMember_deBinarize<T>)>
    Binarizer& deBinarize(T& rhs)
    {
      return
	rhs.deBinarize(*this);
    }
    
    /// Write on the binarizer
    template <typename T,
	      SFINAE_ON_TEMPLATE_ARG(std::is_trivially_copyable_v<T>)>
    Binarizer& binarize(const T& rhs)
    {
      return
	pushBack(&rhs,sizeof(T));
    }
    
    /// Read from the binarizer
    template <typename T,
	      SFINAE_ON_TEMPLATE_ARG(std::is_trivially_copyable_v<T>)>
    Binarizer& deBinarize(T& rhs)
    {
      return
	readAdvancing(&rhs,sizeof(T));
    }
    
    /// Binarize a tuple-like
    template <typename T,
	      SFINAE_ON_TEMPLATE_ARG(isTupleLike<T>)>
    Binarizer& binarize(T&& rhs)     ///< Input
    {
       forEach(rhs,
	      [this](auto& s)
	      {
		this->binarize(s);
	      });
      
      return
	*this;
    }
    
    /// DeBinarize a tuple-like
    template <typename T,
	      SFINAE_ON_TEMPLATE_ARG(isTupleLike<T>)>
    Binarizer& deBinarize(T&& rhs)     ///< Output
    {
      forEach(rhs,
	      [this](auto& s)
	      {
		this->deBinarize(s);
	      });
      
      return
	*this;
    }
    
    /// Binarize a vector-like
    template <typename T,
	      SFINAE_ON_TEMPLATE_ARG(isVectorLike<T>)>
    Binarizer& binarize(T&& rhs)     ///< Input
    {
      /// Number of elements
      const size_t nel=
	rhs.size();
      
      this->binarize(nel);
      
      for(int iel=0;iel<nel;iel++)
	this->binarize(rhs[iel]);
      
      return
	*this;
    }
    
    /// DeBinarize a vector-like
    template <typename T,
	      SFINAE_ON_TEMPLATE_ARG(isVectorLike<T>)>
    Binarizer& deBinarize(T&& rhs)     ///< Output
    {
      /// Number of elements
      size_t nel;
      
      this->deBinarize(nel);
      
      rhs.resize(nel);
      
      for(int iel=0;iel<nel;iel++)
	this->deBinarize(rhs[iel]);
      
      return
	*this;
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
  
  /// Add binarizable functionality via CRTP
  template <typename T>
  class Binarizable
  {
  public:
    
    PROVIDE_CRTP_CAST_OPERATOR(T);
    
    /// Binarize a Serializable
    template <typename B=Binarizer>
    Binarizer& binarize(B&& out={})             ///< Output
      const
    {
      return
	out.binarize(CRTP_THIS());
    }
    
    /// DeBinarize a Serializable
    template <typename B=Binarizer>
    Binarizer& deBinarize(B&& rhs)               ///< Input
    {
      return
	rhs.deBinarize(CRTP_THIS());
    }
  };
}

#endif
