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
  /// Minimal alignment
#define DEFAULT_ALIGNMENT			\
  16
  
  /// Memory manager
  class Memory
  {
    /// List of dynamical allocated memory
    std::map<void*,size_t> used;
    
    /// List of cached allocated memory
    std::map<size_t,std::vector<void*>> cached;
    
    /// Size of used memory
    ValWithMax<size_t> usedSize;
    
    /// Size of cached memory
    ValWithMax<size_t> cachedSize;
    
    /// Use or not cache
    bool useCache{true};
    
    /// Get aligned memory
    ///
    /// Call the system routine which allocate memory
    void* allocateRawAligned(const size_t size,         ///< Amount of memory to allocate
			     const size_t alignment)    ///< Required alignment
    {
      // runLog()<<"Raw allocating "<<size;
      
      /// Result
      void* ptr=
	nullptr;
      
      /// Returned condition
      int rc=
	posix_memalign(&ptr,
		       alignment,
		       size);
      
      if(rc)
	CRASH<<"Failed to allocate "<<size<<" with alignement "<<ALIGNMENT;
      
      return
	ptr;
    }
    
    /// Add to the list of used memory
    void pushToUsed(void* ptr,
		    const size_t size)
    {
      used[ptr]=
	size;
      
      usedSize+=
	size;
      
      // runLog()<<"Pushing to used "<<ptr<<" "<<size<<", number of used:"<<used.size();
    }
    
    /// Removes a pointer from the used list, without actually freeing associated memory
    ///
    /// Returns the size of the memory pointed
    size_t popFromUsed(void* ptr) ///< Pointer to the memory to move to cache
    {
      // runLog()<<"Popping from used "<<ptr;
      
      auto el=
	used.find(ptr);
      
      if(el==used.end())
	CRASH<<"Unable to find dinamically allocated memory "<<ptr;
      
      /// Size of memory
      const size_t size=
	el->second;
      
      usedSize-=
	size;
      
      used.erase(el);
      
      return
	size;
    }
    
    /// Adds a memory to cache
    void pushToCache(const size_t size,
		     void* ptr)
    {
      cached[size].push_back(ptr);
      
      cachedSize+=
	size;
      
      // runLog()<<"Pushing to cache "<<size<<" "<<ptr<<", cache size: "<<cached.size();
    }
    
    /// Pop from the cache, returning to use
    void* popFromCache(const size_t& size,
		       const size_t& alignment)
    {
      // runLog()<<"Try to popping from cache "<<size;
      
      /// List of memory with searched size
      auto list=
	cached.find(size);
      
      if(list==cached.end())
	return
	  nullptr;
      else
	{
	  /// Get latest cached memory
	  void* ptr=
	    list->second.back();
	  
	  if(reinterpret_cast<uintptr_t>(ptr)%alignment!=0)
	    return
	      nullptr;
	  else
	    {
	      list->second.pop_back();
	      
	      cachedSize-=
		size;
	      
	      if(list->second.size()==0)
		cached.erase(list);
	      
	      return
		ptr;
	    }
	}
    }
    
    /// Move the allocated memory to cache
    void moveToCache(void* ptr) ///< Pointer to the memory to move to cache
    {
      // runLog()<<"Moving to cache "<<ptr;
      
      /// Size of pointed memory
      const size_t size=
	popFromUsed(ptr);
      
      pushToCache(size,ptr);
    }
    
  public:
    
    /// Enable cache usage
    void enableCache()
    {
      useCache=
	true;
    }
    
    /// Disable cache usage
    void disableCache()
    {
      useCache=
	false;
      
      clearCache(); 
    }
    
    /// Allocate or get from cache after computing the proper size
    template <class T=char>
    T* getRawAligned(const size_t nel,
		     const size_t alignment)
    {
      /// Total size to allocate
      const size_t size=
	sizeof(T)*nel;
      
      /// Allocated memory
      void* ptr=
	popFromCache(size,alignment);
      
      if(ptr==nullptr)
	ptr=
	  allocateRawAligned(size,alignment);
      
      pushToUsed(ptr,size);
      
      return
	static_cast<T*>(ptr);
    }
    
    /// Decleare unused the memory
    template <class T>
    void release(T* ptr) ///< Pointer getting freed
    {
      if(useCache)
	moveToCache(static_cast<void*>(ptr));
      else
	{
	  popFromUsed(ptr);
	  free(ptr);
	}
    }
    
    /// Release all used memory
    void releaseAllUsedMemory()
    {
      /// Iterator on elements to release
      auto el=
	used.begin();
      
      while(el!=used.end())
	{
	  // runLog()<<"Releasing "<<el.first<<" size "<<el.second;
	  
	  /// Pointer to memory to release
	  void* ptr=
	    el->first;
	  
	  // Increment iterator before releasing
	  el++;
	  
	  release(ptr);
	}
    }
    
    /// Release all memory from cache
    void clearCache()
    {
      /// Iterator to elements of the cached memory list
      auto el=
	cached.begin();
      
      while(el!=cached.end())
	{
	  /// Number of elements to free
	  const size_t n=
	    el->second.size();
	  
	  /// Size to be removed
	  const size_t size=
	    el->first;
	  
	  // Increment before erasing
	  el++;
	  
	  for(size_t i=0;i<n;i++)
	    {
	      // runLog()<<"Removing from cache size "<<el.first;
	      
	      /// Memory to free
	      void* ptr=
		popFromCache(size,DEFAULT_ALIGNMENT);
	      
	      free(ptr);
	    }
	}
    }
    
    /// Destruct the memory manager
    ///
    /// First
    ~Memory()
    {
      releaseAllUsedMemory();
      
      runLog()<<"Maximal memory used: "<<usedSize.extreme()<<" byte, finally used: "<<usedSize;
      
      clearCache();
      
      runLog()<<"Maximal memory cached: "<<cachedSize.extreme()<<" byte, finally used: "<<cachedSize;
    }
  };
  
  extern Memory memory;
  
}

#endif
