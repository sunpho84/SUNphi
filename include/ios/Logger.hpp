#ifndef _LOGGER_HPP
#define _LOGGER_HPP

/// \file Logger.hpp
///
/// \brief Header file to define a logger wrapping FILE*
///
/// The internal class is used to really print, while the external one
/// to determine whether to print or not, and to lock in case the
/// threads are present. If printing is not needed, a fake internal
/// logger is passed.


#include <cstdio>

#include <system/Debug.hpp>
//#include <system/Mpi.hpp>
#include <system/Threads.hpp>
#include <system/Timer.hpp>
#include <utility/Macros.hpp>

namespace SUNphi
{
  /// Write output to a file, using different level of indentation
  class Logger
  {
    /// Single line in the logger
    class LoggerLine
    {
      /// Store the reference to the weather the line has to be ended or not
      bool hasToEnd;
      
      /// Reference to the logger
      const Logger& logger;
      
      /// Forbids copying a line
      LoggerLine(const LoggerLine&)=delete;
      
    public:
      
      /// Starts a new line
      void startNewLine()
      {
	// Prepend with time
	if(logger.prependTime)
	  fprintf(logger.file,"%.010f s:\t",durationInSec(timings.currentMeasure()));
	
	// Writes the given number of spaces
	for(int i=0;i<logger.indentLev;i++)
	  fputc(' ',logger.file);
      }
      
      /// Construct
      LoggerLine(const Logger& logger)
	: hasToEnd(true),logger(logger)
      {
#warning DARIVEDERE
	threads.mutexLock();
	
	startNewLine();
      }
      
      /// Move constructor
      LoggerLine(LoggerLine&& oth)
	: hasToEnd(true),logger(oth.logger)
      {
	oth.hasToEnd=
	  false;
      }
      
      /// Ends the line
      void endLine()
      {
	fputc('\n',logger.file);
      }
      
      /// Destroy (end the line)
      ~LoggerLine()
      {
	if(hasToEnd)
	  {
	    endLine();
#warning DARIVEDERE
	    threads.mutexUnlock();
	  }
      }
      
      /// Prints a string
      LoggerLine& operator<<(const char* str)
      {
	/// Pointer to the first char of the string
	const char* p=
	  str;
	
	// Prints until finding end of string
	while(*p!='\0')
	  {
	    // If new line is found, end current line
	    if(*p=='\n')
	      {
		endLine();
		startNewLine();
	      }
	    else
	      // Prints the char
	      fputc(*p,logger.file);
	    
	    // Increment the char
	    p++;
	  }
	
	return
	  *this;
      }
      
      /// Prints a c++ string
      LoggerLine& operator<<(const std::string& str)
      {
	return
	  (*this)<<str.c_str();
      }
      
      /// Prints a double
      LoggerLine& operator<<(const double& d)
      {
	/// String to print real numbers
	///
	/// The first component is signed or not
	/// The second component is the format
	static constexpr char realFormatString[2][3][6]=
	  {{"%.*g","%.*f","%.*e"},
	   {"%+.*g","%+.*f","%+.*e"}};
	
	fprintf(logger.file,realFormatString[logger.alwaysPrintSign][(int)logger.realFormat],logger.realPrecision,d);
	
	return
	  *this;
      }
    };
    
    /// Indentation level
    int indentLev{0};
    
    /// Determine wheter the new line includes time
    bool prependTime;
    
    /// File pointed by the logger
    FILE* file{nullptr};
    
    /// Set that only master thread can write here
    bool onlyMasterThread{true};
    
    /// Set that only master thread can write here
    bool onlyMasterRank{true};
    
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
    
    /// Create a new line, and print it
    template <typename T>
    LoggerLine operator<<(T&& t)
    {
      /// Check whether current thread should print or not
      const bool thisThreadPrint=
	threads.isMasterThread() or not onlyMasterThread;
      
      /// Check whether current rank should print or not
      // const bool thisRankPrint=
      // 	mpi.isMasterThread() or not onlyMasterThread;
      
      return
	std::move(LoggerLine(*this)<<forw<T>(t));
    }
    
    /// Create with a path
    Logger(const char* path,              ///< Path to open
	   const bool& prependTime=true)  ///< Prepend or not with time
      :
      prependTime(prependTime)
    {
      this->open(path);
      
      *this<<"Logger"<<" initialized";
    }
    
    /// Destroy
    ~Logger()
    {
      // Close if open
      if(isOpen())
	this->close();
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
  
  extern Logger runLog;
}

#endif
