#ifndef _MEMORY_HPP
#define _MEMORY_HPP

/// \file Memory.hpp
///
/// \brief Header file for the allocation and deallocation of memory
/// \todo: implement memory pool and cacher

#include <cstdlib>

#include <debug/Crash.hpp>
#include <ios/Logger.hpp>
#include <system/SIMD.hpp>
#include <utility/ValWithExtreme.hpp>

namespace SUNphi
{
  class Memory
  {
    /// List of dynamical allocated memory
    std::map<void*,size_t> list;
    
    /// Used memory
    ValWithMax<size_t> used;
    
  public:
    
    Memory()
    {
    }
    
    /// Get aligned memory
    ///
    /// Call the system routine which allocate memory
    template <class T=char>
    T* getRawAligned(const size_t nel)
    {
      /// Total size to allocate
      const size_t size=
	sizeof(T)*nel;
      
      /// Result
      void* ptr=
	nullptr;
      
      /// Returned condition
      int rc=
	posix_memalign(&ptr,
		       ALIGNMENT,size);
      
      if(rc)
	CRASH<<"Failed to allocate "<<nel<<" elements of size "<<sizeof(T)<<" with alignement "<<ALIGNMENT;
      else
	{
	  list[ptr]=
	    size;
	  
	  used+=
	    size;
	}
      
      return
	static_cast<T*>(ptr);
    }
    
    /// Free memory and zero the pointer
    template <class T>
    void free(T* &_ptr) ///< Pointer getting freed, set to zero
    {
      void* &ptr=
	reinterpret_cast<void*&>(_ptr);
      
      auto el=
	list.find(ptr);
      
      if(el!=list.end())
	{
	  ::free(ptr);
	  
	  list.erase(el);
	  
	  ptr=
	    nullptr;
	  
	  used-=
	    el->second;
	}
      else
	CRASH<<"Unable to find dinamically allocated memory "<<ptr;
    }
    
    ~Memory()
    {
      if(list.size())
	for(auto& el : list)
	  runLog()<<"Freeing "<<el.first<<" size "<<el.second;
      
      runLog()<<"Maximal memory used: "<<used.extreme()<<" byte, finally used: "<<used;
    }
  };
  
  extern Memory memory;
  
}

#endif
