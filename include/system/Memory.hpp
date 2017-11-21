#ifndef _MEMORY_HPP
#define _MEMORY_HPP

/// \file Memory.hpp
///
/// \brief Header file for the allocation and deallocation of memory

#include <cstdlib>

#include <system/Debug.hpp>

namespace SUNphi
{
  /// Basic alignement for AVX-512, to be generalized
  ///
  static constexpr size_t ALIGNMENT=64;
  
  /// Get aligned memory
  ///
  /// Call the system routine which allocate memory
  ///
  template <class T=char>
  T* getRawAlignedMem(const size_t nel)
  {
    void* ptr;
    int rc=posix_memalign(&ptr,ALIGNMENT,sizeof(T)*nel);
    if(rc) CRASH("Failed to allocate ",nel," elements of size ",sizeof(T),"with alignement",ALIGNMENT);
      
    return static_cast<T*>(ptr);
  }
  
  /// Free memory and zero the pointer
  ///
  /// For the time being it just free the memory, but it would be
  /// interesting to implement a memory pool
  ///
  template <class T>
  void freeMem(T* &ptr) ///< Pointer getting freed, set to zero
  {
    free(ptr);
    ptr=nullptr;
  }
}

#endif