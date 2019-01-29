#ifndef _LOGGER_HPP
#define _LOGGER_HPP

/// \file Logger.hpp
///
/// \brief Header file to define a logger wrapping FILE*

#include <cstdio>

#include <system/Debug.hpp>
#include <system/Timer.hpp>
#include <utility/Macros.hpp>

namespace SUNphi
{
  /// Write output to a file, using different level of indentation
  class Logger
  {
    /// Indentation level
    int indentLev{0};
    
    /// Store if a new line was started
    bool isOnNewLine{true};
    
    /// Determine wheter the new line includes time
    bool prependTime;
    
    /// File pointed by the logger
    FILE* file{nullptr};
    
    /// Ends the line
    void endLine()
    {
      fputc('\n',file);
      
      // Set that we are at the beginning of a new line
      isOnNewLine=
	true;
    }
    
    /// Starts a new line
    void startNewLine()
    {
      // Set that we are not any longer at the beginning of a new line
      isOnNewLine=
	false;
      
      // Prepend with time
      if(prependTime)
	fprintf(file,"%.010f s:\t",durationInSec(timings.currentMeasure()));
      
      // Writes the given number of spaces
      for(int i=0;i<indentLev;i++)
	fputc(' ',file);
    }
    
  public:
    
    /// Precision to print real number
    int realPrecision{6};
    
    /// Flag to determine whether print always or not the sign
    bool alwaysPrintSign{false};
    
    /// Print mode for double/float
    enum class RealFormat{GENERAL=0,FIXED=1,ENGINEER=2};
    
    /// Format mode for real number
    RealFormat realFormat{RealFormat::GENERAL};
    
    /////////////////////////////////////////////////////////////////
    
    /// Increase indentation
    void indentMore()
    {
      indentLev++;
    }
    
    /// Decrease indentation
    void indentLess()
    {
      indentLev--;
    }
    
    /// Open a file, crashing if impossible
    void open(const char* path)
    {
      // Check not open
      if(isOpen())
	CRASH("Cannot open an already opened file");
      
      // Tries to open
      file=
	fopen(path,"w");
      
      // Check
      if(file==nullptr)
	CRASH("Unable to open file",path);
    }
    
    /// Check if open
    bool isOpen()
      const
    {
      return
	file!=nullptr;
    }
    
    /// Close, crashing if not open
    void close()
    {
      // Check that is open
      if(not isOpen())
	CRASH("Not open!");
      
      // Close
      fclose(file);
      
      // Set the file to null
      file=
	nullptr;
    }
    
    /// Create with a path
    Logger(const char* path,              ///< Path to open
	   const bool& prependTime=true)  ///< Prepend or not with time
      :
      prependTime(prependTime)
    {
      this->open(path);
    }
    
    /// Destroy
    ~Logger()
    {
      // Ends the line if not already on the new line
      if(not isOnNewLine)
	endLine();
      
      // Close if open
      if(isOpen())
	this->close();
    }
    
    /// Intercepts the end of line call (might be more complicated actually)
    Logger& operator<<(std::ostream&(*)(std::ostream&))
    {
      endLine();
      
      return
	*this;
    }
    
    /// Prints a string
    Logger& operator<<(const char* str)
    {
      /// Pointer to the first char of the string
      const char* p=
	str;
      
      // Prints until finding end of string
      while(*p!='\0')
	{
	  // If new line is found, end current line
	  if(*p=='\n')
	    endLine();
	  else
	    {
	      // If at the beginning of the new line, indent
	      if(isOnNewLine)
		startNewLine();
	      
	      // Prints the char
	      fputc(*p,file);
	    }
	  
	  // Increment the char
	  p++;
	}
      
      return
	*this;
    }
    
    /// Prints a c++ string
    Logger& operator<<(const std::string& str)
    {
      return
	(*this)<<str.c_str();
    }
    
    /// Prints a double
    Logger& operator<<(const double& d)
    {
      // If at the beginning of the new line, indent
      if(isOnNewLine)
	startNewLine();
      
      /// String to print real numbers
      ///
      /// The first component is signed or not
      /// The second component is the format
      static constexpr char realFormatString[2][3][6]=
	{{"%.*g","%.*f","%.*e"},
	 {"%+.*g","%+.*f","%+.*e"}};
      
      fprintf(file,realFormatString[alwaysPrintSign][(int)realFormat],realPrecision,d);
      
      return
	*this;
    }
  };
  
  /// Increment the logger indentation level for the object scope
  class ScopeIndenter
  {
    /// Pointed logger
    Logger& logger;
    
  public:
    
    /// Create and increase indent level
    ScopeIndenter(Logger& logger) :
      logger(logger)
    {
      logger.indentMore();
    }
    
    /// Delete and decrease indent level
    ~ScopeIndenter()
    {
      logger.indentLess();
    }
  };
  
  /// Mark the logger to be more indented
#define SCOPE_INDENT(VAR)					\
  /*! Indent current scope */					\
  ScopeIndenter NAME2(SCOPE_INDENTER,__LINE__)(VAR)
  
  /// Change the attribute for the object scope
  template <typename T>
  class ScopeChangeAttribute
  {
    /// Reference
    T& ref;
    
    /// Old value
    const T oldVal;
    
  public:
    
    /// Create and increase indent level
    ScopeChangeAttribute(T& ref,        ///< Reference to change
  			 const T& val)  ///< Value to set
      : ref(ref),oldVal(ref)
    {
      // Set the new value
      ref=
  	val;
    }
    
    /// Delete and decrease indent level
    ~ScopeChangeAttribute()
    {
      // Set back the old value
      ref=
  	oldVal;
    }
  };
  
  /// Deduction guide for ScopeChangeAttribute
  template <typename T>
    ScopeChangeAttribute(T& ref,
  			 const T& val)
    -> ScopeChangeAttribute<T>;
  
  /// Set for current scope
#define SET_FOR_CURRENT_SCOPE(VAR,...)				\
  ScopeChangeAttribute NAME2(SET,__LINE__)(VAR,__VA_ARGS__)
  
  /// Set the precision for current scope
#define SCOPE_REAL_PRECISION(LOGGER,VAL)			\
  SET_FOR_CURRENT_SCOPE(LOGGER.realPrecision,VAL)
  
  /// Set printing or not sign at the beginning of a number for current scope
#define SCOPE_ALWAYS_PUT_SIGN(LOGGER)			\
  SET_FOR_CURRENT_SCOPE(LOGGER.alwaysPrintSign,true)
  
  extern Logger logger;
}

#endif
