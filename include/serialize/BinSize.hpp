#ifndef _BINSIZE_HPP
#define _BINSIZE_HPP

/// \file BinSize.hpp
///
/// \brief Report the binary size of specific objects

#include <string>

namespace SUNphi
{
  /// Returns the size to store a string
  inline size_t binSize(const std::string& str)
  {
    return
      sizeof(str.size())+str.length();
  }
}

#endif
