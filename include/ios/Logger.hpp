#ifndef _LOGGER_HPP
#define _LOGGER_HPP

/// \file Logger.hpp
///
/// \brief Header file to define a logger wrapping FILE*

#include <cstdio>

#include <system/Debug.hpp>

namespace SUNphi
{
  /// Write output to a file, using different level of indentation
  class Logger
  {
    /// Indentation level
    int indentLev{0};
    
    /// Store if a new line was started
    bool isOnNewLine;
    
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
      // Writes the given number of spaces
      for(int i=0;i<indentLev;i++)
	fputc(' ',file);
      
      // Set that we are not any longer at the beginning of a new line
      isOnNewLine=
	false;
    }
    
  public:
    
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
    Logger(const char* path)
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
    
    /// Intercepts the end of line call (might be more complicated actually
    Logger& operator<<(std::ostream&(*)(std::ostream&))
    {
      endLine();
      
      return
	*this;
    }
    
    /// Prints a string
    Logger& operator<<(const char* str)
    {
      /// Poiter to the first char of the string
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
  };
  
  extern Logger logger;
}

#endif
