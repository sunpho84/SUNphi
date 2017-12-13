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
  /// BE FIXED)
  template <class B, // Bound type
	    class TG, // Get type
	    class T_TK=typename std::remove_reference_t<B>::TK,
	    class T_TK_TYPES=typename T_TK::Types>
  class Binder :
    public ConstraintIsTensKind<T_TK>,
    public ConstraintTupleHasType<TG,T_TK_TYPES>
  {
    /// Nested type Tensor Kind
    using NestedTK=T_TK;
    
  public:
    
    /// Reference to the object from which to get
    Conditional<B::isStoring,B&,B> ref;
    
    /// Index to get
    int id;
    
    /// Returns whether this TEx is storing
    static constexpr bool isStoring=false;
    
    /// TensorKind of the bound expression
    using TK=typename NestedTK::template AllButType<TG>;
    
    // using InnerTypes=typename NestedTK::template AllAfterType<TG>;
    
    // using OuterTypes=typename NestedTK::template AllBeforeType<TG>;
    
    /// Evaluator
    template <class...Args>
    friend decltype(auto) eval(Binder& binder,const Args&...args)
    {
      return eval(binder.ref,binder.id,std::forward<const Args>(args)...);
    }
    
    /// Constructor taking a rvalue
    Binder(B&& ref,int id) : ref(ref),id(id)
    {
    }
    
    /// Constructor taking lvalue
    Binder(B& ref,int id) : ref(ref),id(id)
    {
    }
    
  };
}

#endif
