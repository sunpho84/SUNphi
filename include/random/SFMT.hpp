#ifndef _SFMT_HPP
#define _SFMT_HPP

/// \file SFMT.hpp
///
/// \brief Implements the SIMD-oriented Fast Mersenne Twister
///
/// http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/SFMT/
///
/// Copyright (c) 2006,2007 Mutsuo Saito, Makoto Matsumoto and Hiroshima
/// University.
/// Copyright (c) 2012 Mutsuo Saito, Makoto Matsumoto, Hiroshima University
/// and The University of Tokyo.
/// All rights reserved.
///
/// Redistribution and use in source and binary forms, with or without
/// modification, are permitted provided that the following conditions are
/// met:
///
///     * Redistributions of source code must retain the above copyright
///       notice, this list of conditions and the following disclaimer.
///     * Redistributions in binary form must reproduce the above
///       copyright notice, this list of conditions and the following
///       disclaimer in the documentation and/or other materials provided
///       with the distribution.
///     * Neither the names of Hiroshima University, The University of
///       Tokyo nor the names of its contributors may be used to endorse
///       or promote products derived from this software without specific
///       prior written permission.
///
/// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
/// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
/// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
/// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
/// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
/// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
/// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
/// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
/// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
/// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
/// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifdef HAVE_CONFIG_H
 #include "config.hpp"
#endif

#include <stdint.h>

#ifdef USE_SSE2
 #include <immintrin.h>
#endif

namespace SUNphi
{
  class SFMTRandomGen
  {
    /// 128-bit data structure
    union w128_t
    {
      uint32_t u[4];
#ifdef USE_SSE2
      __m128i si;
#endif
    };
    
    /// Used to shift left or right
    enum LEFT_RIGHT_T{LEFT,RIGHT};
    
    /// Emulates the SIMD implementation
    template <LEFT_RIGHT_T LeftRight>
    w128_t shift128(const w128_t& in,
		    const int& shift)
    {
      const uint64_t th=
	((uint64_t)in.u[3]<<32)|((uint64_t)in.u[2]);
      
      const uint64_t tl=
	((uint64_t)in.u[1]<<32)|((uint64_t)in.u[0]);
      
      uint64_t oh,ol;
      
      if constexpr(LeftRight==RIGHT)
	{
	  oh=
	    th>>(shift*8);
	  
	  ol=
	    (tl>>(shift*8))|
	    (th<<(64-shift*8));
	}
      else
	{
	  oh=
	    (th<<(shift*8))|
	    (tl>>(64-shift*8));
	  
	  ol=
	    tl<<(shift*8);
	}
      
      return
	{{(uint32_t)ol,
	  (uint32_t)(ol>>32),
	  (uint32_t)oh,
	  (uint32_t)(oh>>32)}};
    }
    
    /// Exponent of the period
    static constexpr int MEXP=
      19937;
    
    /// Number of 128-bit words
    static constexpr int N=
      MEXP/128+1;
    
    /// Number of 32-bit words
    static constexpr int N32=
      N*4;
    
    /// State array
    w128_t state[N];
    
    /// Counter to the 32-bit internal state array
    int idx;
    
    /// Performs the recursion
    void recursion(w128_t& r,
		   w128_t& a,
		   w128_t& b,
		   w128_t& c,
		   w128_t& d)
    {
      static constexpr uint32_t SL1=
	14;
      
      static constexpr uint32_t SL2=
	1;
      
      static constexpr uint32_t SR1=
	11;
      
      static constexpr uint32_t SR2=
	1;
      
      static constexpr w128_t MSK=
	{{0xdfffffefU,
	  0xddfecb7fU,
	  0xbffaffffU,
	  0xbffffff6U}};
      
#ifndef USE_SIMD_MERSENNE_TWISTER
      
      const w128_t x=
	shift128<LEFT>(a,SL2);
      const w128_t y=
	shift128<RIGHT>(c,SR2);
      
      for(int i=0;i<4;i++)
	r.u[i]=
	  a.u[i]^x.u[i]^((b.u[i]>>SR1)&MSK.u[i])
	  ^y.u[i]^(d.u[i]<<SL1);
      
#else
      
      r.si=
	_mm_xor_si128(_mm_xor_si128(_mm_xor_si128(_mm_xor_si128(_mm_srli_si128(c.si,
									       SR2),
								a.si),
						  _mm_slli_epi32(d.si,
								 SL1)),
				    _mm_slli_si128(a.si,
						   SL2)),
		      _mm_and_si128(_mm_srli_epi32(b.si,
						   SR1),
				    MSK.si));
      
#endif
    }
    
  public:
    
    /// Certificate the period of 2^MEXP
    void periodCertification()
    {
      uint32_t inner=0;
      uint32_t *psfmt32=&state[0].u[0];
      
      constexpr uint32_t parity[4]=
		  {0x00000001U,
		   0x00000000U,
		   0x00000000U,
		   0x13c9e684U};
      
      for(int i=0;i<4;i++)
	inner^=
	  psfmt32[i]&parity[i];
      
      for(int i=16;i>0;i>>=1)
	inner^=
	  inner>>i;
      
      inner&=
	1;
      
      /// Check
      bool fixed=
	inner==1;
      
      int i=
	0;
      
      while((not fixed) and (i<4))
	{
	  uint32_t work=
	    1;
	  
	  int j=
	    0;
	  
	  while((not fixed) and (j<32))
	    {
	      if((work&parity[i])!=0)
		{
		  psfmt32[i]^=
		    work;
		  
		  fixed=
		    true;
		}
	      
	      work=
		work<<1;
	      
	      j++;
	    }
	  
	  i++;
	}
    }
    
    /// Generates a new state
    void randomizeAll()
    {
      static constexpr int POS1=
	122;
      
         w128_t &r1=
	state[N-2];
      
      w128_t &r2=
	state[N-1];
      
      for(int i=0;i<N;i++)
	{
	  recursion(state[i],state[i],
		    state[(i+POS1)%N],r1,r2);
	  r1=
	    r2;
	  
	  r2=
	    state[i];
	}
    }
    
    /// Gets the next random number in the stream
    uint32_t operator()()
    {
      /// Proxy to he first element of the state
      uint32_t* psfmt32=
	&state[0].u[0];
      
      if(idx>=N32)
	{
	  randomizeAll();
	  
	  idx=
	    0;
	}
      
      return
	psfmt32[idx++];
    }
    
    /// Seed the state with a 32-bit integer seed
    void seed(const uint32_t& seed)
    {
      uint32_t *psfmt32=
	&state[0].u[0];
      
      psfmt32[0]=
	seed;
      
      for(int i=1;i<N32;i++)
	psfmt32[i]=
	  1812433253UL*
	  (psfmt32[i-1]^
	   (psfmt32[i-1]>>30))
	  +i;
      
      idx=
	N32;
      
      periodCertification();
    }
  };
}

#endif
