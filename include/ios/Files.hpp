#ifndef _FILES_HPP
#define _FILES_HPP

/// \file Files.hpp
///
/// \brief Access to filesystem

#include <filesystem>

namespace SUNphi
{
  /// Returns whether the given path exists
  bool fileExists(const std::filesystem::path& path) ///< Path to open
  {
    return
      std::filesystem::exists(path);
  }
  
  /// Returns the size of a file
  std::uintmax_t fileSize(const std::filesystem::path& path) ///< Path to probe
  {
    return
      std::filesystem::file_size(path);
  }
  
  /// File access, with functionalities to open and close, write/read
  class File
  {
    /// Raw file pointer
    FILE *file;
  };
  
}

#endif
