#ifndef _MEMORY_HPP
#define _MEMORY_HPP

/// \file Memory.hpp
///
/// \brief Header file for the allocation and deallocation of memory

#include <cstdlib>

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
  T* GetRawAlignedMem(const size_t nel)
  {
    void* ptr;
    int rc=posix_memalign(&ptr,ALIGNMENT,sizeof(T)*nel);
    if(rc)
      
    return static_cast<T*>(ptr);
  }
  
}

#endif
