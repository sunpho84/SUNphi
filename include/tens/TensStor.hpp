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
  /// Tensor Kind used to define the structure of a tensor
  ///
  /// The tensor kind defines the list of components of a tensor. It
  /// is used to the define the underlying set of components of a \c
  /// TensorStorage, or the returned type of a Template Expression
  template <class TK,class T>
  class TensStor :
    public ConstraintIsTensKind<TK> //Check that TK is a TensKind
  {
    /// Tuple containg all mapped type
    using type=typename TK::Types;
    
    /// Internal storage
    T *v;
    
  public:
    
    /// Debug access to internal storage
    T* &_v=v;
    
    /// Evaluate a TensStor given a set of components
    template <class...Args>
    friend T& eval(TensStor& ts,Args...args)
    {
      static_assert(IntSeq<IsSame<Args,int>...>::hMul,"All arguments have to be integer");
      
      int id=index<TK>(args...);
      printf("Index: %d\n",id); //debug
      
      return ts.v[id];
    }
    
    /// Constructor (test)
    TensStor()
    {
      static_assert(TK::nDynamic==0,"Dynamic case not implemented");
      size_t nel=10;
      
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
