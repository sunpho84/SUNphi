#ifndef _TENSSTOR_HPP
#define _TENSSTOR_HPP

/// \file TensStor.hpp
///
/// \brief Header file for the definition of a storage space for a tensor

#include <system/Memory.hpp>
#include <tens/Indexer.hpp>
#include <tens/TensKind.hpp>

namespace SUNphi
{
  /// Tensor Storage holding the resources for a tensor
  ///
  /// The tensor storage allocates and deallocates the memory location
  /// where a tensor is materially stored, keeping track of the amount
  /// of memory allocated. Facilities to reallocate the memory are
  /// provided
  template <class TK,class T>
  class TensStor :
    public ConstraintIsTensKind<TK> // Check that TK is a TensKind
  {
    /// Tuple containg all mapped type
    using type=typename TK::Types;
    
    /// Internal storage
    T* v;
    
  public:
    
    /// Debug access to internal storage
    T* &_v=v;
    
    /// Returns a non-const reference to a TensStor given a set of components
    template <class...Args>
    friend T& eval(TensStor& ts,const Args&...args)
    {
      static_assert(IntSeq<IsSame<Args,int>...>::hMul,"All arguments have to be integer");
      
      const int id=index<TK>(std::forward<const Args>(args)...);
      //printf("Index: %d\n",id); //debug
      
      return ts.v[id];
    }
    
    /// Returns a const reference to a TensStor given a set of components
    template <class...Args>
    friend const T& eval(const TensStor& ts,const Args&...args)
    {
      static_assert(IntSeq<IsSame<Args,int>...>::hMul,"All arguments have to be integer");
      
      const int id=index<TK>(std::forward<const Args>(args)...);
      //printf("Index: %d\n",id); //debug
      
      return ts.v[id];
    }
    
    /// Constructor (test)
    TensStor()
    {
      static_assert(TK::nDynamic==0,"Dynamic case not implemented");
      size_t nel=TK::maxStaticIdx;
      
      v=getRawAlignedMem<T>(nel); //allocate
    }
    
    /// Destructor
    ~TensStor()
    {
      freeMem(v);
    }
  };
  
}

#endif
