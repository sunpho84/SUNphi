#ifndef _UNARYMINUS_HPP
#define _UNARYMINUS_HPP

/// \file UnaryMinus.hpp
///
/// \brief Header file for the definition of -smet

#include <physics/Spin.hpp>
#include <smet/NnarySmET.hpp>
#include <tens/TensKind.hpp>

namespace SUNphi
{
  // Base type to qualify as UMinuser
  DEFINE_BASE_TYPE(UMinuser);
  
  /// Class to take the conjugate of a SmET
  template <typename..._Refs>                               // Type of the expression to conjugate
  class UMinuser :
    public BaseUMinuser,                                // Inherit from BaseUMinuser to detect in expression
    public NnarySmET<UMinuser<_Refs...>>,               // Inherit from NnarySmET
    public ConstrainAreSmETs<_Refs...>                  // Constrain all \c _Refs to be \c SmETs
  {
  public:
    
    PROVIDE_NNARY_SMET_REFS_AND_CHECK_ARE_N(1);
    
    /// Returns the opposite of the argument
    template <typename T>                         // Type of the argument to negate
    static DECLAUTO representativeFunction(T&& t) ///< Argument to negate
    {
      return -t;
    }
    
    PROVIDE_SIMPLE_NNARY_COMP_SIZE;
    
    // Attributes
    NOT_STORING;
    FORWARD_IS_ALIASING_TO_REFS;
    
    SAME_TK_AS_REF(0);
    
    NO_EXTRA_MERGE_DELIMS;
    
    REPRESENTATIVE_FUNCTION_WINS_ALL;
    
    PROVIDE_NNARY_SMET_SIMPLE_CREATOR(UMinuser);
  };
  
  // Check that a test UMinuser is a NnarySmET
  STATIC_ASSERT_IS_NNARY_SMET(UMinuser<Tens<TensKind<Spin>,double>>);
  
  // Build UMinuser from uminus
  SIMPLE_NNARY_SMET_BUILDER(uMinus,UMinuser);
  
  // Simplifies uMinus(uMinus)
  CANCEL_DUPLICATED_NNARY_SMET_CALL(uMinus,UMinuser);
  
  /// Implement -smet: return uminus
  template <typename T,              // Type of the expression
	    SFINAE_ON_TEMPLATE_ARG(isSmET<T>)>
  DECLAUTO operator-(T&& smet)       ///< Expression
  {
    return uMinus(forw<T>(smet));
  }
}

#endif
