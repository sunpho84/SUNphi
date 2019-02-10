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
#include <ios/TextColors.hpp>
#include <system/Mpi.hpp>
#include <system/Timer.hpp>
#include <utility/Macros.hpp>
#include <utility/ScopeDoer.hpp>

namespace SUNphi
{
  /// Write output to a file, using different level of indentation
  class Logger
  {
    /// Fake logger, printing to /dev/null
    static Logger fakeLogger;
    
    /// Single line in the logger
    class LoggerLine
    {
      /// Store wether the line has to be ended or not
      bool hasToEndLine;
      
      /// Store whether the line is crashing
      bool hasToCrash;
      
      /// Mark that the color has changed in this line
      bool colorChanged;
      
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
	  rc+=
	    fprintf(logger.file,"%.010f s",durationInSec(timings.currentMeasure()));
	
	// Prepend with rank
	if(someOtherRankCouldBePrinting)
	  rc+=
	    fprintf(logger.file," Rank %d",mpi.rank());
	
	// Prepend with thread
	if(someOtherThreadCouldBePrinting)
	  rc+=
	    fprintf(logger.file," Thread %d",threads.getThreadId());
	
	// Mark the margin
	if(rc)
	  fprintf(logger.file,":\t");
	
	// Writes the given number of spaces
	for(int i=0;i<logger.indentLev;i++)
	  fputc(' ',logger.file);
      }
      
      /// Construct
      LoggerLine(Logger& logger)
	: hasToEndLine(true),
	  hasToCrash(false),
	  colorChanged(false),
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
	    
	    if(hasToLock)
	      logger.releaseExclusiveAccess();
	    
	    if(hasToCrash)
	      {
		printBacktraceList();
		exit(1);
	      }
	  }
      }
      
      /// Print a C-style variadic message
      template <int MAX_LENGTH=256>  // Maximal length to be be printed
      LoggerLine& printVariadicMessage(const char* format, ///< Format to print
				       va_list ap)         ///< Variadic part
      {
	/// Message to be printed
	char message[MAX_LENGTH];
	
	/// Resulting length if the space had been enough
	int rc=
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
	
	(*this)<<TextColor::RED<<" ERROR in function "<<cr.getFuncName()<<" at line "<<cr.getLine()<<" of file "<<cr.getPath()<<": \"";
	
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
      
      /// Prints an unsigned integer
      LoggerLine& operator<<(const uint& i)
      {
	fprintf(logger.file,"%u",i);
	
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
      
      /// Prints a pointer
      template <typename T>
      LoggerLine& operator<<(const T* p)
      {
	fprintf(logger.file,"%p",p);
	
	return
	  *this;
      }
      
      /// Prints after putting a space
      template <typename T>
      LoggerLine& operator*(T&& t)
      {
	return
	  *this<<' '<<forw<T>(t);
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
    
    /// File pointed by the logger
    FILE* file{nullptr};
    
  public:
    
    /// Decide whether only master thread can write here
    bool onlyMasterThreadPrint{true};
    
    /// Decide whether only master MPI can write here
    bool onlyMasterRankPrint{true};
    
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
	CRASH<<"Cannot open an already opened file";
      
      // Tries to open
      file=
	fopen(path,"w");
      
      // Check
      if(file==nullptr)
	CRASH<<"Unable to open file "<<path;;
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
    
    /// Create a new line
    LoggerLine getNewLine()
    {
      return
	*this;
    }
    
    /// Create a new line, and print on it
    template <typename T>
    LoggerLine operator<<(T&& t)
    {
      return
	std::move(getNewLine()<<forw<T>(t));
    }
    
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
      this->open(path);
      
      // Cannot print, otherwise all rank would!
      //*this<<"Logger initialized";
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
  
  /// Set for current scope
#define SET_FOR_CURRENT_SCOPE(NAME,VAR,...)			\
  ScopeChangeVar NAME3(SET,NAME,__LINE__)(VAR,__VA_ARGS__)
  
  /// Set the precision for current scope
#define SCOPE_REAL_PRECISION(LOGGER,VAL)			\
  SET_FOR_CURRENT_SCOPE(LOGGER_REAL_PRECISION,LOGGER.realPrecision,VAL)
  
  /// Set printing or not sign at the beginning of a number for current scope
#define SCOPE_ALWAYS_PUT_SIGN(LOGGER)			\
  SET_FOR_CURRENT_SCOPE(LOGGER_ALWAYS_PRINT_SIGN,LOGGER.alwaysPrintSign,true)
  
  extern Logger runLog;
}

#endif
