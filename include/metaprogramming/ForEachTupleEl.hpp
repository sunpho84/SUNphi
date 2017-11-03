#ifndef _FOREACHTUPLEEL_HPP
#define _FOREACHTUPLEEL_HPP

/// \file ForEachTupleEl.hpp
///
/// \brief Loop over all Tuple elements

#include <metaprogramming/EnableIf.hpp>
#include <metaprogramming/Tuple.hpp>

namespace SUNphi
{
  /// Loop over all \c Tuple elements
  ///
  /// Terminator of recursive call, called automaticaly also when the
  /// Tuple is empty.
  ///
  /// \tparam I Index of the \c Tuple element to extract
  /// \tparam Func \c Function type
  /// \tparam Tp... The Tuple parameters
  /// \return \c void
  ///
  template<size_t I=0,typename Func,typename...Tp>
  VoidIf<I==sizeof...(Tp)>
  ForEach(const Tuple<Tp...>& t ///< \c Tuple to act upon
	  ,Func f)              ///< \c Function iterating on the Tuple
  {
  }
  
  /// Loop over all \c Tuple elements
  ///
  /// Non const access to all elements of the \c Tuple, called
  /// recursively until I==sizeof...(Tp), incrementing the parameter
  /// I.
  ///
  /// \tparam I Index of the \c Tuple element to extract
  /// \tparam Func \c Function type
  /// \tparam Tp... The \c Tuple parameters
  /// \return \c void
  ///
  template<size_t I=0,typename Func,typename...Tp>
  VoidIf<(I<sizeof...(Tp))>
  ForEach(Tuple<Tp...>& t ///< \c Tuple to act upon
	  ,Func f)        ///< \c Function iterating on the \c Tuple
  {
    f(Get<I>(t));
    ForEach<I+1,Func,Tp...>(t,f);
  }
  
  /// Loop over all \c tuple elements
  ///
  /// Const access to all elements of the \c Tuple, called recursively
  /// until I==sizeof...(Tp), incrementing the parameter I.
  ///
  /// \tparam I Index of the \c Tuple element to extract
  /// \tparam Func \c Function type
  /// \tparam Tp... The \c Tuple parameters
  /// \return \c void
  ///
  template<size_t I=0,typename Func,typename...Tp>
  VoidIf<(I<sizeof...(Tp))>
  ForEach(const Tuple<Tp...>& t ///< \c Tuple to act upon
	  ,Func f)              ///< \c Function iterating on the \c Tuple
  {
    f(Get<I>(t));
    ForEach<I+1,Func,Tp...>(t,f);
  }
}

#endif
