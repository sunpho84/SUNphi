#ifndef SITMO_HPP
#define SITMO_HPP

/// \file Sitmo.hpp
///
/// \brief Implements the SITMO random number generator
///
///  Copyright (c) 2012-2016 M.A. (Thijs) van den Berg, http://sitmo.com/
///
///  Use, modification and distribution are subject to the MIT Software License.
///
///  The MIT License (MIT)
///  Permission is hereby granted, free of charge, to any person obtaining a copy
///  of this software and associated documentation files (the "Software"), to deal
///  in the Software without restriction, including without limitation the rights
///  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
///  copies of the Software, and to permit persons to whom the Software is
///  furnished to do so, subject to the following conditions:
///
///  The above copyright notice and this permission notice shall be included in
///  all copies or substantial portions of the Software.
///
///  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
///  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
///  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
///  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
///  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
///  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
///  THE SOFTWARE.

#include <random/Encrypter.hpp>

namespace SUNphi
{
  namespace Sitmo
  {
    /// Class encapsulating the Sitmo random generator
    DEFINE_SERIALIZABLE_CLASS(Rng)
    {
      /// Default seed
      static constexpr uint32_t DEFAULT_SEED=
	3472291050;
      
      /// Holds the state of the generator
      class State :
	public Sitmo::Word
      {
	
      public:
	
	/// Increment of a certain amount
	State operator+(const uint64_t& z)
	  const
	{
	  /// Result
	  State out=
	    (*this);
	  
	  // Increment
	  out[0]=
	    (*this)[0]+z;
	  
	  // Overflow check
	  if(out[0]<=(*this)[0])
	    {
	      /// Digit to check for overflow
	      int iDigit=
		1;
	      
	      // Carry over
	      do out[iDigit]++;
	      while(out[iDigit++]==0 and iDigit<4);
	    }
	  
	  return
	    out;
	}
	
	/// Self increment
	State operator+=(const uint64_t& z)
	{
	  return
	    (*this)=
	    (*this)+z;
	}
	
	/// Unitary self-increment
	State operator++()
	{
	  return
	    (*this)+=
	    1;
	}
      };
      
      
      /// Seed, used to encrypt
      SERIALIZABLE_SEQUENCE_WITH_TAG(Key,key,"seed");
      
      /// State of the random number generator
      SERIALIZABLE_SEQUENCE(State,state);
      
      /// Number of reserved random number
      uint64_t nReserved{0};
      
      /// State whether the rng is locked or not
      bool locked{false};
      
      /// Lock the generator
      bool lock()
      {
	if(locked)
	  CRASH<<"Trying to lock an already locked generator!";
	
	locked=
	  true;
	
	return
	  locked;
      }
      
      /// Unlock the generator
      bool unLock()
      {
	if(not locked)
	  CRASH<<"Trying to unlock a non-locked generator!";
	
	locked=
	  false;
	
	return
	  locked;
      }
      
    public:
      
      /// Type returned
      using ResultType=
	uint32_t;
      
      LIST_SERIALIZABLE_MEMBERS(key,state);
      
      /// Draw a seed from a random generator
      template <typename F>
      void seed(F& f)
      {
	/// Result type of the callabale generator
	using FRes=
	  typename F::ResultType;
	
	/// Number of calls to be issued to fill the state
	constexpr int nCall=
	  sizeof(Word)/sizeof(FRes);
	
	union
	{
	  /// Partial key
	  Word partKey;
	  
	  /// Access to the key in a way which allows to fill with the generator
	  FRes rawKey[nCall];
	};
	
	/// Fill the key
	for(int i=0;i<nCall;i++)
	  rawKey[i]=
	    f();
	
	key()=
	  buildKey(partKey);
      }
      
      /// Seed from a seed
      void seed(const uint32_t& s)
      {
	/// Partial word to be used
	const Word& partKey=
	  {s};
	
	key()=
	  buildKey(partKey);
      }
      
      /// Generate a number with a given offset w.r.t current state
      ResultType generateNth(const uint64_t& offset)
      {
	// Check that this is in the range reserved
	if(offset>=nReserved)
	  CRASH<<"Asking to generate "<<offset<<", beyond the reserved range: "<<nReserved;
	
	union
	{
	  /// Temporary result of the encyipter
	  Word temp;
	  
	  /// Output to be returned
	  const ResultType out{};
	};
	
	/// Shifted state
	State shiftedState=
	  state+offset;
	
	temp=
	  encrypt(key,static_cast<Word>(shiftedState));
	
	return
	  out;
      }
      
      /// Reserve a number of random number
      bool reserve(const uint64_t& n)
      {
	nReserved=
	  n;
	
	return
	  lock();
      }
      
      /// Release the reserved number of random number, increasing the state
      bool release()
      {
	state+=
	  nReserved;
	
	nReserved=
	  0;
	
	return
	  unLock();
      }
      
      /// Reserves, returning a scope-locker
      ///
      /// Automatically releases the lock and advances when the locker
      /// goes out of scope
      [[ nodiscard ]]
	auto scopeReserve(const uint64_t& n)
      {
	return
	  ScopeDoer([this,n]()
		    {
		      return
			this->reserve(n);
		    },
		    [this]()
		    {
		      return
			this->release();
		    });
      }
      
      /// Skip n numbers
      void skip(const uint64_t& n)
      {
	auto s=
	  scopeReserve(n);
      }
      
      /// Generate a number in the current stream and increase of 1
      ResultType generateNext()
      {
	/// Holds the lock on the stream
	auto hold=
	  scopeReserve(1);
	
	return
	  generateNth(0);
      }
      
      /// Default constructor
      explicit Rng(const uint32_t& s=DEFAULT_SEED)
      {
	seed(s);
      }
    };
  }
}

#endif
