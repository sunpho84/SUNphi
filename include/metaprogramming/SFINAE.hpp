#ifndef _SFINAE_HPP
#define _SFINAE_HPP

/// \file SFINAE.hpp
///
/// \brief Defines macros to enable or disable template instantiation
///
/// http://en.cppreference.com/w/cpp/language/sfinae

namespace SUNphi
{
  /// Provides a SFINAE to be used in template par list
  ///
  /// This follows
  /// https://stackoverflow.com/questions/32636275/sfinae-with-variadic-templates
  /// as in this example
  /// \code
  /// template <typename D,
  ///           SFINAE_ON_TEMPLATE_ARG(IsSame<D,int>)>
  /// void foo(D i) {} // fails if D is not int
  /// \endcode
#define SFINAE_ON_TEMPLATE_ARG(...)	\
  TypeIf<(__VA_ARGS__),void*> =nullptr
  
  /// Provides template par list to unprioritize default SFINAE
  ///
  /// Use as last argument of a function overloaded by a other
  /// implementations using SFINAE to detect the proper version to be
  /// used. This has to be used in conjunction with the other macros
  /// SFINAE_WORSEN_DEFAULT_VERSION_ARGS and
  /// SFINAE_WORSEN_DEFAULT_VERSION_ARGS_CHECK as in this example
  ///
  /// \code
  /// template <typename T,
  ///           SFINAE_WORSEN_DEFAULT_VERSION_TEMPLATE_PARS>
  /// int tell(T a,SFINAE_WORSEN_DEFAULT_VERSION_ARGS)
  /// {
  ///   SFINAE_WORSEN_DEFAULT_VERSION_ARGS_CHECK;
  ///   return 1;
  /// }
  ///
  /// template <typename T,
  ///           std::enable_if_t<sizeof(T)==4,void*> =nullptr>
  /// decltype(auto) tell(T&& a)
  /// {
  ///   return a+1;
  /// }
  ///
  /// int main()
  /// {
  ///    tell(1); //returns 2
  ///
  ///    return 0;
  /// }
  ///\endcode
#define SFINAE_WORSEN_DEFAULT_VERSION_TEMPLATE_PARS			\
  typename...DummyTypes                      /* Fake list of types                  */ \
  
  /// Provide empty list of args, used to unprioritize default version
#define SFINAE_WORSEN_DEFAULT_VERSION_ARGS				\
  DummyTypes...     /*< Fake list of args */				\
  
  /// Check that no extra arg is passed
#define SFINAE_WORSEN_DEFAULT_VERSION_ARGS_CHECK	\
  STATIC_ASSERT_ARE_N_TYPES(0,DummyTypes)
  
  //////////////////////////////////////////////////////////////////////
  
  // Not sure that this is a good ide
  // #define IF17(...)   if constexpr(__VA_ARGS__)
  
  // #define ELSE17	 else
}

#endif
