#ifndef _LOGGER_HPP
#define _LOGGER_HPP

/// \file Logger.hpp
///
/// \brief Header file to define a logger wrapping FILE*
///
/// The internal class is used to really print, while the external one
/// to determine whether to print or not, and to lock in case the
/// threads are present. If printing is not needed, a fake internal
/// logger is passed, printing on /dev/null
///
/// If threads are running, and all threads are allowed to print, the
/// Logger is locked so that only one thread at the time can print,
/// and all lines are prepended with thread id indication.
///
/// If MPI is running, and all ranks are allowed, to print, each line
/// is prepended with the rank id. No cross-ranks lock is issued.

#include <cstdio>

#include <Threads.hpp>
#include <debug/BackTracing.hpp>
#include <debug/Crash.hpp>
#include <ios/File.hpp>
#include <ios/TextFormat.hpp>
#include <system/Mpi.hpp>
#include <system/Timer.hpp>
#include <utility/Macros.hpp>
#include <utility/ScopeDoer.hpp>

namespace SUNphi
{
  /// Write output to a file, using different level of indentation
  class Logger :
    private File
  {
    /// Fake logger, printing to /dev/null
    static Logger fakeLogger;
    
    /// Access to the logger as if it was a file
    const File& file()
      const
    {
      return
	static_cast<const File&>(*this);
    }
    
    PROVIDE_ALSO_NON_CONST_METHOD(file);
    
    /// Single line in the logger
    class LoggerLine
    {
      /// Store wether the line has to be ended or not
      bool hasToEndLine;
      
      /// Store whether the line is crashing
      bool hasToCrash;
      
      /// Mark that the color has changed in this line
      bool colorChanged;
      
      /// Mark that the style has changed in this line
      bool styleChanged;
      
      /// Check whether should actually print or not
      const bool reallyPrint;
      
      /// Check if some other rank could be printing
      const bool someOtherRankCouldBePrinting;
      
      /// Check if some other thread could be printing
      const bool someOtherThreadCouldBePrinting;
      
      /// Check whether we need to lock
      const bool hasToLock;
      
      /// Reference to the logger
      Logger& logger;
      
      /// Forbids copying a line
      LoggerLine(const LoggerLine&)=delete;
      
    public:
      
      /// Starts a new line
      void startNewLine()
      {
	/// Total number of the character written
	int rc=
	  0;
	
 	// Prepend with time
	if(logger.prependTime)
	  {
	    SCOPE_REAL_FORMAT_FIXED(logger);
	    SCOPE_REAL_PRECISION(logger,10);
	    SCOPE_ALWAYS_PRINT_ZERO(logger);
	    SCOPE_NOT_ALWAYS_PUT_SIGN(logger);
	    rc+=
	      (logger.file()<<durationInSec(timings.currentMeasure())<<" s").getRc();
	  }
	
	// Prepend with rank
	if(someOtherRankCouldBePrinting)
	  rc+=
	    (logger.file()<<" Rank "<<mpi.rank()).getRc();
	
	// Prepend with thread
	if(someOtherThreadCouldBePrinting)
	  rc+=
	    (logger.file()<<" Thread "<<threads.getThreadId()).getRc();
	
	// Mark the margin
	if(rc)
	  logger.file()<<":\t";
	
	// Writes the given number of spaces
	for(int i=0;i<logger.indentLev;i++)
	  logger.file()<<' ';
      }
      
      /// Construct
      LoggerLine(Logger& logger)
	: hasToEndLine(true),
	  hasToCrash(false),
	  colorChanged(false),
	  styleChanged(false),
	  reallyPrint((threads.isMasterThread() or not logger.onlyMasterThreadPrint) and (mpi.isMasterRank() or not logger.onlyMasterRankPrint)),
	  someOtherRankCouldBePrinting(mpi.nRanks()!=1 and not logger.onlyMasterRankPrint),
	  someOtherThreadCouldBePrinting(threads.nActiveThreads()!=1 and not logger.onlyMasterThreadPrint),
	  hasToLock(reallyPrint and someOtherThreadCouldBePrinting),
	  logger(reallyPrint?logger:fakeLogger)
      {
	
	if(hasToLock)
	  logger.getExclusiveAccess();
	  
	startNewLine();
      }
      
      /// Move constructor
      LoggerLine(LoggerLine&& oth)
      	: hasToEndLine(true),
      	  hasToCrash(oth.hasToCrash),
	  colorChanged(oth.colorChanged),
	  styleChanged(oth.styleChanged),
	  reallyPrint(oth.reallyPrint),
	  someOtherRankCouldBePrinting(oth.someOtherRankCouldBePrinting),
	  someOtherThreadCouldBePrinting(oth.someOtherThreadCouldBePrinting),
	  hasToLock(oth.hasToLock),
      	  logger(oth.logger)
      {
      	oth.hasToEndLine=
      	  false;
      }
      
      /// Ends the line
      void endLine()
      {
	logger.file()<<'\n';
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
	    
	    // Reset style
	    if(styleChanged)
	      *this<<TextStyle::RESET;
	    
	    // Ends the line
	    endLine();
	    
	    if(hasToLock)
	      logger.releaseExclusiveAccess();
	    
	    if(hasToCrash)
	      {
		printBacktraceList();
		exit(1);
	      }
	  }
      }
      
      /// Prints after putting a space
      template <typename T>               // Type of the obected to print
      LoggerLine& operator*(T&& t)      ///< Object to be printed
      {
	logger.file()*forw<T>(t);
	
	return
	  *this;
      }
      
      /// Catch-all print
      ///
      /// The SFINAE is needed to avoid that the method is used when
      /// File does not know how to print
      template <typename T,                                       // Type of the quantity to print
		typename=EnableIf<not canPrint<LoggerLine,T>>,    // SFINAE needed to avoid ambiguous overload
		typename=EnableIf<canPrint<File,const T&>>>       // SFINAE needed to avoid ambiguous overload
      LoggerLine& operator<<(const T& t)                          ///< Object to print
      {
	logger.file()<<t;
	
	return
	  *this;
      }
      
      /// Print a C-style variadic message
      LoggerLine& printVariadicMessage(const char* format, ///< Format to print
				       va_list ap)         ///< Variadic part
      {
	logger.file().printVariadicMessage(format,ap);
	
	return
	  *this;
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
      
      /// Changes the style of the line
      LoggerLine& operator<<(const TextStyle& c)
      {
	styleChanged=
	  true;
	
	return
	  *this<<
	    TEXT_CHANGE_STYLE_HEAD<<
	    static_cast<char>(c)<<
	    TEXT_CHANGE_STYLE_TAIL;
      }
      
      /// Prints crash information
      ///
      /// Then sets the flag \c hasToCrash to true, such that at
      /// destroy of the line, crash happens
      LoggerLine& operator<<(const Crasher& cr)
      {
	this->hasToCrash=
	  true;
	
	(*this)<<TextColor::RED<<" ERROR in function "<<cr.getFuncName()<<" at line "<<cr.getLine()<<" of file "<<cr.getPath()<<": \"";
	
	return
	  *this;
      }
      
      /// Prints a string, parsing newline
      LoggerLine& operator<<(const char* str)
      {
	if(str==nullptr)
	  logger.file()<<str;
      else
	{
	  /// Pointer to the first char of the string
	  const char* p=
	    str;
	  
	  // Prints until finding end of string
	  while(*p!='\0')
	    {
	      // starts a new line
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
	}
	
	return
	  *this;
      }
      
      /// Prints a c++ string
      LoggerLine& operator<<(const std::string& str)
      {
	return
	  *this<<str.c_str();
      }
      
    };
    
    /// Indentation level
    int indentLev{0};
    
    /// Determine wheter the new line includes time
    bool prependTime;
    
    /// Mutex used to lock the logger
    mutable Mutex mutex;
    
    /// Set the exclusive access right
    void getExclusiveAccess()
    {
      mutex.lock();
    }
    
    /// Release the exclusive access right
    void releaseExclusiveAccess()
    {
      mutex.unlock();
    }
    
  public:
    
    using File::alwaysPrintSign;
    using File::alwaysPrintZero;
    using File::realFormat;
    using File::realPrecision;
    
    /// Decide whether only master thread can write here
    bool onlyMasterThreadPrint{true};
    
    /// Decide whether only master MPI can write here
    bool onlyMasterRankPrint{true};
    
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
    
    /// Create a new line
    LoggerLine getNewLine()
    {
      return
	*this;
    }
    
    /// Create a new line
    LoggerLine operator()()
    {
      return
	*this;
    }
    // /// Create a new line, and print on it
    // template <typename T,
    // 	      typename=EnableIf<canPrint<LoggerLine,T>>,            // SFINAE needed to avoid ambiguous overload
    // 	      typename=EnableIf<not canPrint<Logger,RemRef<T>>>>    // SFINAE to avoid ambiguous reimplementation
    // LoggerLine operator<<(T&& t)
    // {
    //   return
    // 	std::move(getNewLine()<<forw<T>(t));
    // }
    
    /// Print a C-style variadic message
    LoggerLine printVariadicMessage(const char* format, ///< Format to print
				    va_list ap)         ///< Variadic part
    {
      return
	std::move(getNewLine().printVariadicMessage(format,ap));
    }
    
    /// Create with a path
    Logger(const char* path,              ///< Path to open
	   const bool& prependTime=true)  ///< Prepend or not with time
      :
      prependTime(prependTime)
    {
      file().open(path,"w");
      
      // Cannot print, otherwise all rank would!
      //*this<<"Logger initialized";
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
  
  extern Logger runLog;
}

/// Create the line
#define RUNLOG \
  runLog()


#endif
