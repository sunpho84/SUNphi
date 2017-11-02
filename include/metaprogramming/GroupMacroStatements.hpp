#ifndef _GROUPMACROSTATEMENTS_HPP
#define _GROUPMACROSTATEMENTS_HPP

/// \file GroupMacroStatements.hpp
///
/// \brief Header file for the macro to wrap a multi-statement macro


/// Group a multi-statements macro to avoid breaking if-else clause
///
/// Example:
///
/// \code
/// #define SWAP(A,B)							\
///   GROUP_MACRO_STATEMENTS						\
///   (									\
///    decltype(A) TMP=A;						\
///    A=B;								\
///    B=TMP;								\
///   )
///
/// int a=5,b=2;
/// if(a>b) SWAP(a,b);
/// \endcode
///
#define GROUP_MACRO_STATEMENTS(...)		\
  do						\
    {						\
      __VA_ARGS__				\
    }						\
  while(0)

#endif
