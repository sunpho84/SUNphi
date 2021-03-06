#ifndef _REFERENCE_HPP
#define _REFERENCE_HPP

/// \file Reference.hpp
///
/// \brief Defines a class which defines a reference to another SmET
///
/// \todo This should be the place where the instantiation depending
/// on the cost of the expression is made

#include <smet/BaseSmET.hpp>

namespace SUNphi
{
  /// Reference to T or new type, depending on wheter the class isStoring
  ///
  /// Const qualifier is passed throughout
  /// \todo add functionality as in Eigen
  template <typename T,                                // Type to get reference from
	    typename=ConstrainIsSmET<T>,               // Constrain the type to be a SmET
	    bool IsStoring=Unqualified<T>::isStoring,  // Check if it is storing
	    bool IsLvalue=isLvalue<T>>                 // Check if lvalue
  using Reference=
    RefIf<IsStoring and IsLvalue,RemRef<T>>;
  
  /////////////////////////////////////////////////////////////////
  
  /// Provide the reference to the object
#define PROVIDE_SMET_REF(NUM)				\
  /*! Type of the binding reference NUM */		\
  using Ref ## NUM=					\
    _Ref ## NUM;					\
  /*! Reference to the NUM object */			\
  Reference<Ref ## NUM> ref ## NUM
}

#endif
