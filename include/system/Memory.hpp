#ifndef _MEMORY_HPP
#define _MEMORY_HPP

/// \file Memory.hpp
///
/// \brief Header file for the allocation and deallocation of memory
/// \todo: implement memory pool and cacher

#include <cstdlib>

#include <debug/Crash.hpp>
#include <system/SIMD.hpp>

namespace SUNphi
{
  /// Get aligned memory
  ///
  /// Call the system routine which allocate memory
  template <class T=char>
  T* getRawAlignedMem(const size_t nel)
  {
    void* ptr;
    int rc=posix_memalign(&ptr,ALIGNMENT,sizeof(T)*nel);
    if(rc)
      CRASH<<"Failed to allocate "<<(int64_t)nel<<" elements of size "<<(int64_t)sizeof(T)<<" with alignement "<<(int64_t)ALIGNMENT;
      
    return static_cast<T*>(ptr);
  }
  
  /// Free memory and zero the pointer
  ///
  /// For the time being it just free the memory, but it would be
  /// interesting to implement a memory pool
  template <class T>
  void freeMem(T* &ptr) ///< Pointer getting freed, set to zero
  {
    free(ptr);
    ptr=nullptr;
  }
}

#endif
