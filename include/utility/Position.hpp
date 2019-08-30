#ifndef _POSITION_HPP
#define _POSITION_HPP

/// \file Position.hpp
///
/// \brief Defines a few constant useful to deal with positions

namespace SUNphi
{
  /// Lament the absence of an element
  [[ maybe_unused ]]
  static constexpr bool
  ASSERT_IF_NOT_PRESENT=
    true;
  
  /// Do not lament the absence of an element
  [[ maybe_unused ]]
  static constexpr bool
  DO_NOT_ASSERT_IF_NOT_PRESENT=
    false;
  
  /// Mark the absence of an element
  [[ maybe_unused ]]
  static constexpr int
  NOT_PRESENT=
    -1;
  
  /// Filterer which check presence of its parameter
  template <auto I>
  struct IsPresent
  {
    /// Result of the check
    static constexpr bool res=
      (I!=NOT_PRESENT);
  };
  
  /// Enumerate first and last
  enum FIRST_OR_LAST{FIRST,LAST};
  
  /// Enumerate the possibity to loop backward or forward
  enum BACK_FORW{BACK,FORW};
}

#endif
