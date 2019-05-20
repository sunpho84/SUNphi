#ifndef ENCRYPTER_HPP
#define ENCRYPTER_HPP

/// \file Encrypter.hpp
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

#include <Serialize.hpp>
#include <metaprogramming/LoopUnroll.hpp>

namespace SUNphi
{
  namespace Sitmo
  {
    /// Type of the key
    using Key=
      std::array<uint64_t,5>;
    
    /// Encrypted word
    using Word=
      std::array<uint64_t,4>;
    
    /// Encrypts the input
    inline Word encrypt(const Key& key,  ///< Key to encrypt
			Word x)          ///< Input to encrypt
    {
      loopUnroll<0,5>([&x,&key](const int j)
  		      {
  			loopUnroll<0,2>([j,&x,&key](const int i)
  					{
  					  constexpr uint64_t mk[2][2]=
  					    {{14,16},{25,33}};
					  
  					  uint64_t& x0=
  					    x[2*i];
  					  uint64_t& x1=
  					    x[(2*i+1)%4];
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
  			loopUnroll<0,3>([j,&x](const int l)
  					{
  					  loopUnroll<0,2>([l,j,&x](const int i)
  							  {
  							    constexpr uint64_t m[2][3][2]=
  							      {{{52,57},{23,40},{5,37}},
  							       {{46,12},{58,22},{32,32}}};
							    
  							    uint64_t& x0=
  							      x[2*i];
  							    uint64_t& x1=
  							      x[(2*i+((l%2==0)?3:1))%4];
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
      
      // Increment entry 3
      x[3]+=
  	5;
      
      return
  	x;
    }
    
    /// Build a key from a word
    Key buildKey(const Word& word)
    {
      /// Output
      Key key;
      
      // Copy the first 4
      for(int i=0;i<4;i++)
       key[i]=
  	  word[i];
      
      // Set the fifth
      key[4]=
  	0x1BD11BDAA9FC1A22^key[0]^key[1]^key[2]^key[3];
      
      return
	key;
    }
  };
}

#endif
