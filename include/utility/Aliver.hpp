#ifndef _ALIVER_HPP
#define _ALIVER_HPP

/// \file Aliver.hpp
///
/// \brief Makes sure that the library is started

namespace SUNphi
{
  /// Helper to make sure that the initializing functions are referenced
  int aliverHelper();
  
  /// Makes sure that the library is started
  ///
  /// Elaborates on the approach of https://stackoverflow.com/a/43352830
  /// 1) takes the returned value of the helper, which is never used
  /// 2) static ensures that every TU gets a separate instance
  static int makeSureAliverIsReferenced=
    aliverHelper();
  
  class Aliver;
  
  extern Aliver aliver;
}

#endif
