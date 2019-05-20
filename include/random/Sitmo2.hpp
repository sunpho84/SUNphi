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
      SERIALIZABLE_SEQUENCE(Sitmo::Key,key);
      
      SERIALIZABLE_SEQUENCE(Sitmo::Word,state);
      
    public:
      
      /// Type returned
      using ResultType=
	uint32_t;
      
      LIST_SERIALIZABLE_MEMBERS(key,state);
      
      /// Put a seed from a random generator
      template <typename F>
      void seed(F& f)
      {
	/// Result type of the callabale generator
	using FRes=
	  typename F::ResultType;
	
	/// Number of calls to be issued to fill the stste
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
    };
  }
}

#endif
