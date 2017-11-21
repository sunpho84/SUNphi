#ifndef _BIND_HPP
#define _BIND_HPP

/// \file Bind.hpp
///
/// \brief Defines a class which binds a component of a TEx

#include <tens/TensKind.hpp>

namespace SUNphi
{
  /// Class to bind a component of a TEx
  ///
  /// The creator must accepts any type which qualifies as a TEx (TO
  /// BE FIXED) CHANGE the name of the bound type to B or Bound, and G
  /// for Get type
  ///
  template <class T,class TG,
	    class T_TK=typename std::remove_reference_t<T>::TK,
	    class T_TK_TYPES=typename T_TK::Types>
  class Binder :
    public ConstraintIsTensKind<T_TK>,
    public ConstraintTupleHasType<TG,T_TK_TYPES>
  {
    /// Nested type Tensor Kind
    using NestedTK=T_TK;
    
  public:
    
    /// Reference to the object from which to get
    Conditional<T::isStoring,T&,T> ref;
    
    /// Index to get
    int id;
    
    static constexpr bool isStoring=false;
    
    /// TensorKind of the bound expression
    using TK=typename NestedTK::template AllButType<TG>;
    
    // using InnerTypes=typename NestedTK::template AllAfterType<TG>;
    
    // using OuterTypes=typename NestedTK::template AllBeforeType<TG>;
    
    /// Evaluator
    template <class...Args>
    friend decltype(auto) eval(Binder &binder,Args...args)
    {
      return eval(binder.ref,binder.id,args...);
    }
    
    /// Creator taking a rvalue
    Binder(T&& ref,int id) : ref(ref),id(id)
    {
    }
    
    // Binder(T& ref,int id) : ref(ref),id(id)
    // {
    // }
    
  };
}

#endif
