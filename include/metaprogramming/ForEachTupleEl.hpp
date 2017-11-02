#ifndef _FOREACHTUPLEEL_HPP
#define _FOREACHTUPLEEL_HPP

/// \file ForEachTupleEl.hpp
///
/// \brief Header file to loop over all tuple elements

#include <tuple>

namespace SUNphi
{
  /// Loop over all \c tuple elements
  ///
  /// Terminator of recursive call
  ///
  template<size_t I=0,typename Func,typename... Tp>
  std::enable_if_t<I==sizeof...(Tp),void>
  ForEach(const std::tuple<Tp...>&,Func)
  {
  }
  
  /// Loop over all \c tuple elements
  ///
  /// Non const access
  ///
  template<size_t I=0,typename Func,typename... Tp>
  std::enable_if_t< I<sizeof...(Tp),void>
  ForEach(std::tuple<Tp...>& t,Func f)
  {
    f(std::get<I>(t));
    ForEach<I+1,Func,Tp...>(t,f);
  }
  
  /// Loop over all \c tuple elements
  ///
  /// Const access
  ///
  template<size_t I=0,typename Func,typename... Tp>
  std::enable_if_t< I<sizeof...(Tp),void>
  ForEach(const std::tuple<Tp...>& t,Func f)
  {
    f(std::get<I>(t));
    ForEach<I+1,Func,Tp...>(t,f);
  }
}

#endif
