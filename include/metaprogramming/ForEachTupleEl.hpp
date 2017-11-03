#ifndef _FOREACHTUPLEEL_HPP
#define _FOREACHTUPLEEL_HPP

/// \file ForEachTupleEl.hpp
///
/// \brief Header file to loop over all tuple elements

#include <metaprogramming/EnableIf.hpp>
#include <metaprogramming/Tuple.hpp>

namespace SUNphi
{
  /// Loop over all \c tuple elements
  ///
  /// Terminator of recursive call, called automaticaly also when the
  /// tuple is empty.
  ///
  /// \return void
  ///
  template<size_t I=0,typename Func,typename...Tp>
  VoidIf<I==sizeof...(Tp)>
  ForEach(const Tuple<Tp...>& t ///< Tuple to act upon
	  ,Func f)              ///< Function iterating on the tuple
  {
  }
  
  /// Loop over all \c tuple elements
  ///
  /// Non const access to all elements of the tuple, called
  /// recursively until I==sizeof...(Tp), incrementing the parameter
  /// I.
  ///
  /// \return void
  ///
  template<size_t I=0,typename Func,typename...Tp>
  VoidIf<(I<sizeof...(Tp))>
  ForEach(Tuple<Tp...>& t ///< Tuple to act upon
	  ,Func f)        ///< Function iterating on the tuple
  {
    f(Get<I>(t));
    ForEach<I+1,Func,Tp...>(t,f);
  }
  
  /// Loop over all \c tuple elements
  ///
  /// Const access to all elements of the tuple, called recursively
  /// until I==sizeof...(Tp), incrementing the parameter I.
  ///
  /// \return void
  ///
  template<size_t I=0,typename Func,typename...Tp>
  VoidIf<(I<sizeof...(Tp))>
  ForEach(const Tuple<Tp...>& t ///< Tuple to act upon
	  ,Func f)              ///< Function iterating on the tuple
  {
    f(Get<I>(t));
    ForEach<I+1,Func,Tp...>(t,f);
  }
}

#endif
