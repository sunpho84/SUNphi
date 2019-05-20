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

#include <array>
#include <cstdint>
#include <iostream>

#include <metaprogramming/LoopUnroll.hpp>

/// Maximal value of the prng output
#define SITMO_RAND_MAX sitmo::prng_engine::max()

namespace SUNphi
{
  /// Class encapsulating the Sitmo random generator
  class Sitmo
  {
    /// Key
    std::array<uint64_t,5> key;
    
    /// State (counter)
    std::array<uint64_t,4> state;
    
    /// Cipher output: 4*64 bit=256 bit output
    union
    {
      std::array<uint64_t,4> state;
      std::array<uint32_t,8> output;
    } ciphered;
    
    /// Output chunk counter: determines wihch 32 of the 256 random bits in cipheredState is returned
    unsigned short iChunk;
    
    /// Encrypts the counter, to set the state
    void encryptCounter(std::array<uint64_t,4>& outputState) ///< Output state
      const
    {
      /// Copy of the state, to be modififed
      uint64_t work[4];
      for(int i=0;i<4;i++)
	work[i]=
	  state[i];
      
      loopUnroll<0,5>([&work,this](const int j)
		      {
			loopUnroll<0,2>([j,&work,this](const int i)
					{
					  constexpr uint64_t mk[2][2]=
					    {{14,16},{25,33}};
					  
					  uint64_t& x0=
					    work[2*i];
					  uint64_t& x1=
					    work[(2*i+1)%4];
					  const uint64_t& rx=
					    mk[j%2][i];
					  
					  x1+=
					    key[(2*i+1+j)%5]+((i==1)?j:0);
					  x0+=
					    x1+key[(2*i+j)%5];
					  x1=
					    (x1<<rx)|(x1>>(64-rx));
					  x1^=
					    x0;
					});
			loopUnroll<0,3>([j,&work](const int l)
					{
					  loopUnroll<0,2>([l,j,&work](const int i)
							  {
							    constexpr uint64_t m[2][3][2]=
							      {{{52,57},{23,40},{5,37}},
							       {{46,12},{58,22},{32,32}}};
							    
							    uint64_t& x0=
							      work[2*i];
							    uint64_t& x1=
							      work[(2*i+((l%2==0)?3:1))%4];
							    const uint64_t& rx=
							      m[j%2][l][i];
							    
							    x0+=
							      x1;
							    x1=
							      (x1<<rx)|(x1>>(64-rx));
							    x1^=
							      x0;
							  });
					});
		      });
      
      // Copy the result to the output
      for(int i=0;i<4;i++)
	outputState[i]=
	  work[i]+key[i];
      
      // Increment entry 3
      outputState[3]+=
	5;
    }
    
  public:
    
    /// Maximal output
    static constexpr uint32_t max=
      0xFFFFFFFF;
    
    /// Copy constructor
    Sitmo(const Sitmo& oth) =///< Other engine
      default;
    
    /// Construct from a seed
    Sitmo(const uint32_t& s=0)
    {
      seed(s);
    }
    
    /// Sets the key
    void setKey(const uint32_t& k0=0,
		const uint32_t& k1=0,
		const uint32_t& k2=0,
		const uint32_t& k3=0)
    {
      key[0]=
	k0;
      key[1]=
	k1;
      key[2]=
	k2;
      key[3]=
	k3;
      key[4]=
	0x1BD11BDAA9FC1A22^key[0]^key[1]^key[2]^key[3];
    }
    
    /// Init using the passed seed
    void seed(const uint32_t& s=0)
    {
      for(int i=0;i<4;i++)
	{
	  key[i]=0;
	  state[i]=0;
	}
      
      setKey(s);
      iChunk=0;
      
      encryptCounter(ciphered.state);
    }
    
    // // req: 26.5.1.4 Random number engine requirements, p.908 table 117, row 8
    // // Advances e’s state ei to ei+1 = TA(ei) and returns GA(ei).
    // uint32_t operator()()
    // {
    //   // can we return a value from the current block?
    //   if (iChunk < 8) {
    // 	unsigned short cipheredState_index = iChunk >> 1;
    // 	iChunk++;
    // 	if (iChunk&1) 
    // 	  return cipheredState[cipheredState_index] & 0xFFFFFFFF; 
    // 	else
    // 	  return cipheredState[cipheredState_index] >> 32;
    //   }
      
    //   // generate a new block and return the first 32 bits
    //   (*this)++;
    //   encryptCounter(cipheredState);
    //   iChunk = 1; // the next call
    //   return cipheredState[0] & 0xFFFFFFFF;   // this call
    // }
    
    /// Returns one of the 8 chunks
    uint32_t operator()()
    {
      if(iChunk>=8)
    	{
    	  // Generate a new block and return the first 32 bits
    	  (*this)++;
    	  encryptCounter(ciphered.state);
	  
	  // Reset the pointer to the beginnning of the chunk
	  iChunk=
	    0;
    	}
      
      return
	ciphered.output[iChunk++];
    }
    
    // -------------------------------------------------
    // misc
    // -------------------------------------------------
    
    /// Advances e’s state ei to ei+z
    void discard(uint64_t z)
    {
      // check if we stay in the current block
      // cast to unsigned int to prevent comparison warning
      if (z < (uint64_t)(8 - iChunk)) {
	iChunk += static_cast<unsigned short>(z);
	return;
      }
      
      // we will have to generate a new block...
      z -= (8 - iChunk);  // discard the remainder of the current blok
      iChunk = z % 8;     // set the pointer in the correct element in the new block
      z -= iChunk;        // update z
      z >>= 3;                // the number of buffers is elements/8
      ++z;                    // and one more because we crossed the buffer line
      (*this)+=z;
      encryptCounter(ciphered.state);
    }
    
    /// Output
    template<class CharT, class Traits>
    friend std::basic_ostream<CharT,Traits>&
    operator<<(std::basic_ostream<CharT,Traits>& os, const Sitmo& s) {
      for (unsigned short i=0; i<4; ++i)
	os << s.key[i] << ' ' << s.state[i] << ' ' << s.ciphered.state[i] << ' ';
      os << s.iChunk;
      return os;
    }
    
    /// Input
    template<class CharT, class Traits>
    friend std::basic_istream<CharT,Traits>&
    operator>>(std::basic_istream<CharT,Traits>& is, Sitmo& s) {
      for (unsigned short i=0; i<4; ++i)
	is >> s.key[i] >> s.state[i] >> s.ciphered.state[i];
      is >> s.iChunk;
      return is;
    } 
    
    /// Check that the two generators are the same
    bool operator==(const Sitmo& y)
    {
      if (iChunk != y.iChunk) return false;
      for (unsigned short i=0; i<4; ++i) {
	if (state[i] != y.state[i]) return false;
	if (key[i] != y.key[i]) return false;
	if (ciphered.state[i] != y.ciphered.state[i]) return false;
      }
      return true;
    }
    
    /// Check that the two generators are different
    bool operator!=(const Sitmo& y)
    {
      return !(*this == y);
    }
    
    /// Set the counter
    void set_counter(uint64_t s0=0, uint64_t s1=0, uint64_t s2=0, uint64_t s3=0, unsigned short o_counter=0)
    {
      state[0] = s0; 
      state[1] = s1; 
      state[2] = s2; 
      state[3] = s3;
      iChunk = o_counter % 8;
      encryptCounter(ciphered.state);
    }
    
    /// Increments the counter by a given amount
    Sitmo& operator+=(const uint64_t z)  ///< Amount to increment
    {
      /// Take note of old value, to check for overflow
      const uint64_t old0=
	state[0];
      
      // Increment
      state[0]+=
	z;
      
      // Overflow check
      if(state[0]<=old0)
	{
	  /// Digit to increment
	  int iDigit=
	    1;
	  
	  // Carry over
	  do state[iDigit]++;
	  while(state[iDigit++]==0 and iDigit<4);
	}
      
      return
	*this;
    }
    
    /// Unitary increment
    Sitmo& operator++(int)
    {
      return
	*this+=
	1;
    }
  };
}

#endif
