#ifndef _SINGLE_INSTANCE_HPP
#define _SINGLE_INSTANCE_HPP

#include <system/Debug.hpp>
#include <metaprogramming/StaticMemberWithInitializator.hpp>

namespace SUNphi
{
  /// A simple class, with a counter wich enable only one instance at the time
  ///
  template <typename T> // Type from which inheriting, to distinguish each count
  class SingleInstance
  {
    
    PROVIDE_STATIC_MEMBER_WITH_INITIALIZATOR(int,count, 0,Counter of instances);
    
    /// Check that no instance exists
    void crashIfInstancesExists()
    {
      if(count()!=0)
	CRASH("Count is not zero:",count);
    }
    
  public:
    
    /// Constructor, checking no instance exists before
    SingleInstance()
    {
      crashIfInstancesExists();
      
      count()++;
    }
    
    /// Destructor, checking that no instance exists at the end
    ~SingleInstance()
    {
      count()--;
      
      crashIfInstancesExists();
    }
  };
}

#endif
