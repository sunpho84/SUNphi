#ifndef _BIND_HPP
#define _BIND_HPP

/// \file Bind.hpp
///
/// \brief Defines a class which binds a component of the TEx

#include <tens/TensKind.hpp>

namespace SUNphi
{
  template <class T,class TG,
	    class T_TK=typename std::remove_reference_t<T>::TK,
	    class T_TK_TYPES=typename T_TK::Types>
  class Binder :
    public ConstraintIsTensKind<T_TK>,
    public ConstraintTupleHasType<TG,T_TK_TYPES>
  {
    using InternalTK=T_TK; ///< Internal type
    
  public:
    
    Conditional<T::isStoring,T&,T> ref;        ///< Reference to the object from which to get
    
    int id;        ///< Index to get
    
    static constexpr bool isStoring=false;
    
    using TK=typename InternalTK::template AllButType<TG>;
    
    // using InnerTypes=typename InternalTK::template AllAfterType<TG>;
    
    // using OuterTypes=typename InternalTK::template AllBeforeType<TG>;
    
    template <class...Args>
    friend decltype(auto) eval(Binder &binder,Args...args)
    {
      return eval(binder.ref,binder.id,args...);
    }
    
    Binder(T&& ref,int id) : ref(ref),id(id)
    {
    }
    
    // Binder(T& ref,int id) : ref(ref),id(id)
    // {
    // }
    
  };
}

#endif
