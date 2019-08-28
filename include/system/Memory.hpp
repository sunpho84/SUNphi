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
    
    /// Number of unaligned allocation performed
    size_t nUnalignedAlloc{0};
    
    /// Number of aligned allocation performed
    size_t nAlignedAlloc{0};
    
    /// Number of cached memory reused
    size_t nCachedReused{0};
    
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
      
      nAlignedAlloc++;
      
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
      
      /// Iterator to search result
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
    void pushToCache(void* ptr,          ///< Memory to cache
		     const size_t size)  ///< Memory size
    {
      cached[size].push_back(ptr);
      
      cachedSize+=
	size;
      
      // runLog()<<"Pushing to cache "<<size<<" "<<ptr<<", cache size: "<<cached.size();
    }
    
    /// Check if a pointer is suitably aligned
    static bool isAligned(const void* ptr,
			  const size_t alignment)
    {
      return
	reinterpret_cast<uintptr_t>(ptr)%alignment==0;
    }
    
    /// Pop from the cache, returning to use
    void* popFromCache(const size_t& size,
		       const size_t& alignment)
    {
      // runLog()<<"Try to popping from cache "<<size;
      
      /// List of memory with searched size
      auto cachedIt=
	cached.find(size);
      
      if(cachedIt==cached.end())
	return
	  nullptr;
      else
	{
	  /// Vector of pointers
	  auto& list=
	    cachedIt->second;
	  
	  /// Get latest cached memory with appropriate alignment
	  auto it=
	    list.end()-1;
	  
	  while(it!=list.begin()-1 and not isAligned(*it,alignment))
	    it--;
	  
	  if(it==list.begin()-1)
	    return
	      nullptr;
	  else
	    {
	      /// Returned pointer, copied here before erasing
	      void* ptr=
		*it;
	      
	      list.erase(it);
	      
	      cachedSize-=
		size;
	      
	      if(list.size()==0)
		cached.erase(cachedIt);
	      
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
      
      pushToCache(ptr,size);
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
    T* provideAligned(const size_t nel,
		      const size_t alignment)
    {
      /// Total size to allocate
      const size_t size=
	sizeof(T)*nel;
      
      /// Allocated memory
      void* ptr;
      
      // Search in the cache
      ptr=
	popFromCache(size,alignment);
      
      // If not found in the cache, allocate new memory
      if(ptr==nullptr)
	ptr=
	  allocateRawAligned(size,alignment);
      else
	nCachedReused++;
      
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
    
    /// Print to a stream
    template <typename T>
    auto& printStatistics(T&& stream)
    {
      return
	stream<<"Maximal memory used: "<<usedSize.extreme()<<" bytes, currently used: "<<usedSize
	      <<" bytes, number of allocation: "<<nUnalignedAlloc<<" unaligned, "<<nAlignedAlloc<<" aligned\n"
	      <<"Maximal memory cached: "<<cachedSize.extreme()<<" bytes, currently used: "<<cachedSize
	      <<" bytes, number of reused: "<<nCachedReused;
    }
    
    /// Create the memory manager
    Memory()
    {
      runLog()<<"Starting the memory manager";
    }
    
    /// Destruct the memory manager
    ~Memory()
    {
      runLog()<<"Stopping the memory manager";
      
      SCOPE_INDENT(runLog);
      
      printStatistics(runLog());
      
      releaseAllUsedMemory();
      
      clearCache();
    }
  };
  
  extern Memory memory;
  
}

#endif
