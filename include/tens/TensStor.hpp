#ifndef _TENSSTOR_HPP
#define _TENSSTOR_HPP

/// \file TensStor.hpp
///
/// \brief Header file for the definition of a storage space for a tensor

#include <tens/TensKind.hpp>
#include <system/Memory.hpp>

namespace SUNphi
{
  /// Tensor Kind used to define the structure of a tensor
  ///
  /// The tensor kind defines the list of components of a tensor. It
  /// is used to the define the underlying set of components of a \c
  /// TensorStorage, or the returned type of a Template Expression
  ///
  template <class TK,class T>
  class TensStor :
    public ConstraintIsTensKind<TK> //Check that TK is a TensKind
  {
    using type=typename TK::type;  /// Tuple containg all mapped type
    
    T *v; ///< Internal storage
    
  public:
    
    /// Constructor (test)
    ///
    TensStor()
    {
      static_assert(TK::nDynamic==0,"Dynamic case not implemented");
      size_t nel=10;
      
      v=getRawAlignedMem<T>(nel);
    }
    
    /// Destructor
    ///
    ~TensStor()
    {
      free(v);
      v=nullptr;
    }
  };
  
}

#endif
