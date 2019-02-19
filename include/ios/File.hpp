#ifndef _FILE_HPP
#define _FILE_HPP

/// \file File.hpp
///
/// \brief Access to filesystem

#include <filesystem>

#include <metaprogramming/UniversalReferences.hpp>
#include <debug/MinimalCrash.hpp>

namespace SUNphi
{
  /// Mark the stream to be more indented
#define SCOPE_INDENT(VAR)					\
  /*! Indent current scope */					\
  ScopeIndenter NAME2(SCOPE_INDENTER,__LINE__)(VAR)
  
  /// Set for current scope
#define SET_FOR_CURRENT_SCOPE(NAME,VAR,...)			\
  ScopeChangeVar NAME3(SET,NAME,__LINE__)(VAR,__VA_ARGS__)
  
  /// Set the precision for current scope
#define SCOPE_REAL_PRECISION(STREAM,VAL)					\
  SET_FOR_CURRENT_SCOPE(STREAM_REAL_PRECISION,STREAM.realPrecision,VAL)
  
  /// Set printing or not sign at the beginning of a number for current scope
#define SCOPE_ALWAYS_PUT_SIGN(STREAM)			\
  SET_FOR_CURRENT_SCOPE(STREAM_ALWAYS_PRINT_SIGN,STREAM.alwaysPrintSign,true)
  
  /// Allows all ransk to print for current scope
#define SCOPE_ALL_RANKS_CAN_PRINT(STREAM)			\
  SET_FOR_CURRENT_SCOPE(STREAM_ALL_RANKS_CAN_PRINT,STREAM.onlyMasterRankPrint,false)
  
  /// Returns whether the given path exists
  inline bool fileExists(const std::filesystem::path& path) ///< Path to open
  {
    return
      std::filesystem::exists(path);
  }
  
  /// Returns the size of a file
  inline std::uintmax_t fileSize(const std::filesystem::path& path) ///< Path to probe
  {
    return
      std::filesystem::file_size(path);
  }
  
  /// File access, with functionalities to open and close, write/read
  class File
  {
    /// Raw file pointer
    FILE *file{nullptr};
    
    /// Returned value of last i/o function
    int rc{0};
    
  public:
    
    /// Open a file, crashing if impossible
    void open(const char* path,              /// Path to open
	      const char* mode,              /// Mode used to open
	      const bool crashIfFail=true)   /// Crash or not if failing
    {
      // Check not open
      if(isOpen())
	MINIMAL_CRASH("Cannot open an already opened file");
      
      // Tries to open
      file=
	fopen(path,mode);
      
      // Check
      if(file==nullptr and crashIfFail)
	MINIMAL_CRASH("Unable to open file %s",path);
    }
    
    /// Check if open
    bool isOpen()
      const
    {
      return
	file!=nullptr;
    }
    
    /// Close
    void close()
    {
      if(isOpen())
	{
	  fclose(file);
	  
	  // Set the file to null
	  file=
	    nullptr;
	}
    }
    
    /// Returns rc
    int getRc()
      const
    {
      return
	rc;
    }
    
    /// Precision to print real number
    int realPrecision{6};
    
    /// Flag to determine whether print always or not the sign
    bool alwaysPrintSign{false};
    
    /// Print mode for double/float
    enum class RealFormat{GENERAL=0,FIXED=1,ENGINEER=2};
    
    /// Format mode for real number
    RealFormat realFormat{RealFormat::GENERAL};
    
    /// Prints a char
    File& operator<<(const char& c) ///< Char to write
    {
      // Prints the char
      rc=
	fputc(c,file);
      
      return
	*this;
    }
    
    /// Prints an integer
    File& operator<<(const int& i) ///< Integer to write
    {
      rc=
	fprintf(file,"%d",i);
      
      return
	*this;
    }
    
    /// Prints an unsigned integer
    File& operator<<(const uint& i) ///< Unsigned integer to write
    {
      rc=
	fprintf(file,"%u",i);
      
      return
	*this;
    }
    
    /// Prints a long integer
    File& operator<<(const int64_t& l) ///< Long integer to write
    {
      rc=
	fprintf(file,"%ld",l);
      
      return
	*this;
    }
    
    /// Prints a double
    File& operator<<(const double& d)
    {
      /// String to print real numbers
      ///
      /// The first component is signed or not
      /// The second component is the format
      static constexpr char realFormatString[2][3][6]=
	{{"%.*g","%.*f","%.*e"},
	 {"%+.*g","%+.*f","%+.*e"}};
      
      rc=
	fprintf(file,realFormatString[alwaysPrintSign][(int)realFormat],realPrecision,d);
      
      return
	*this;
    }
    
    /// Prints a pointer
    template <typename T>           // Type pointed
    File& operator<<(const T* p)  ///< Pointer to be printed
    {
      rc=
	fprintf(file,"%p",p);
      
      return
	*this;
    }
    
    /// Prints after putting a space
    template <typename T>         // Type of the obected to print
    File& operator*(T&& t)      ///< Object to be printed
    {
      return
	*this<<' '<<forw<T>(t);
    }
    
    /// Prints a string
    File& operator<<(const char* str)
    {
      if(str==nullptr)
	return
	  *this<<"(null)";
      else
	{
	  /// Pointer to the first char of the string
	  const char* p=
	    str;
	  
	  // Prints until finding end of string
	  while(*p!='\0')
	    {
	      // Prints the char
	      *this<<*p;
	      
	      // Increment the char
	      p++;
	    }
	  
	  return
	    *this;
	}
    }
    
    /// Prints a c++ string
    File& operator<<(const std::string& str)
    {
      return
	(*this)<<str.c_str();
    }
    
    /// Print a C-style variadic message
    template <int MAX_LENGTH=256>  // Maximal length to be be printed
    File& printVariadicMessage(const char* format, ///< Format to print
			       va_list ap)         ///< Variadic part
    {
      /// Message to be printed
      char message[MAX_LENGTH];
      
      /// Resulting length if the space had been enough
      rc=
	vsnprintf(message,MAX_LENGTH,format,ap);
      
      /// Check if it was truncated
      bool truncated=
	(rc<0 or rc>=MAX_LENGTH);
      
      if(truncated)
	*this<<message<<" (truncated line)";
      else
	*this<<message;
      
      return
	*this;
    }
    
    /// Destroy
    ~File()
    {
      // Close if open
      if(isOpen())
	this->close();
    }
    
  };
}

#endif
