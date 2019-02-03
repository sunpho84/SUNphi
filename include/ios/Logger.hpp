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
/// \todo REVIEW THIS, I think we will short circuit the printing

#include <cstdio>

#include <ios/TextColors.hpp>
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
      /// Store wether the line has to be ended or not
      bool hasToEndLine;
      
      /// Store wether the line is crashing
      bool hasToCrash;
      
      /// Mark that the color has changed in this line
      bool colorChanged;
      
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
	: hasToEndLine(true),
	  hasToCrash(false),
	  colorChanged(false),
	  logger(logger)
      {
#warning DARIVEDERE
	threads.mutexLock();
	
	startNewLine();
      }
      
      /// Move constructor
      LoggerLine(LoggerLine&& oth)
      	: hasToEndLine(true),
      	  hasToCrash(oth.hasToCrash),
	  colorChanged(oth.colorChanged),
      	  logger(oth.logger)
      {
      	oth.hasToEndLine=
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
	// Wrap everything here
	if(hasToEndLine)
	  {
	    // Ends the quoted text
	    if(hasToCrash)
	      *this<<"\"";
	    
	    // Reset color
	    if(colorChanged)
	      *this<<TextColor::DEFAULT;
	    
	    // Ends the line
	    endLine();
#warning DARIVEDERE
	    threads.mutexUnlock();
	    
	    if(hasToCrash)
	      {
		printBacktraceList();
		exit(1);
	      }
	  }
      }
      
      /// Changes the color of the line
      LoggerLine& operator<<(const TextColor& c)
      {
	colorChanged=
	  true;
	
	return
	  *this<<
	    TEXT_CHANGE_COLOR_HEAD<<
	    static_cast<char>(c)<<
	    TEXT_CHANGE_COLOR_TAIL;
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
	      *this<<*p;
	    
	    // Increment the char
	    p++;
	  }
	
	return
	  *this;
      }
      
      /// Prints a char
      LoggerLine& operator<<(const char& c)
      {
	// Prints the char
	fputc(c,logger.file);
	
	return
	  *this;
      }
      
      /// Prints a c++ string
      LoggerLine& operator<<(const std::string& str)
      {
	return
	  (*this)<<str.c_str();
      }
      
      /// Prints crash information
      ///
      /// Then sets the flag \c hasToCrash to true, such that at
      /// destroy of the line, crash happens
      LoggerLine& operator<<(const Crasher& cr)
      {
	this->hasToCrash=
	  true;
	
	(*this)<<TextColor::RED<<" ERROR in function "<<cr.funcName<<" at line "<<cr.line<<" of file "<<cr.path<<": \"";
	
	return
	  *this;
      }
      
      /// Prints an integer
      LoggerLine& operator<<(const int& i)
      {
	fprintf(logger.file,"%d",i);
	
	return
	  *this;
      }
      
      /// Prints a long integer
      LoggerLine& operator<<(const int64_t& l)
      {
	fprintf(logger.file,"%ld",l);
	
	return
	  *this;
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
