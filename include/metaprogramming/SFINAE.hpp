#ifndef _SFINAE_HPP
#define _SFINAE_HPP

/// \file SFINAE.hpp
///
/// \brief Defines macros to enable or disable template instantiation
///
/// http://en.cppreference.com/w/cpp/language/sfinae

#include <metaprogramming/TypeTraits.hpp>

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
  typename...DummyTypes       /* Fake list of types                  */
  
  /// Provide empty list of args, used to unprioritize default version
#define SFINAE_WORSEN_DEFAULT_VERSION_ARGS	\
  DummyTypes...     /*< Fake list of args */
  
  /// Check that no extra arg is passed
#define SFINAE_WORSEN_DEFAULT_VERSION_ARGS_CHECK	\
  STATIC_ASSERT_ARE_N_TYPES(0,DummyTypes)
  
  //////////////////////////////////////////////////////////////////////
  
  /// SFINAE for template class specialisation
  ///
  /// Mechanism to allow the usage of SFINAE to allow a class specialization.
  /// Follows the advice of https://stackoverflow.com/a/30991097 to
  /// create a class providing the type itself
  ///
  /// Example:
  ///
  /// \code
  ///
  /// Class which needs to be explicitly specialized with SFINAE
  /// template <typename T>
  /// class SpecializableClass
  /// {
  /// };
  ///
  /// /// Dummy type to be used to specialize the class
  /// class SpecializingArg
  /// {
  /// };
  ///
  /// PROVIDE_ENABLE_IF_FOR_TYPE(SpecializingArg);
  ///
  /// /// Specializes the class SpecializableClass
  /// SFINAE_TEMPLATE_CLASS_SPECIALIZATION_PREAMBLE
  /// class SpecializableClass<SFINAE_TEMPLATE_CLASS_SPECIALIZATION_ARG(SpecializingArg)>
  ///  {
  ///    SFINAE_TEMPLATE_CLASS_SPECIALIZATION_PROVIDE_TYPE;
  ///
  ///    /// To show explicit usage of T
  ///    SpecializableClass<T>()
  ///    {
  ///    }
  ///  };
  ///
  /// \endcode
  ///
#define SFINAE_TEMPLATE_CLASS_SPECIALIZATION_PREAMBLE		       \
  template <template<typename...> typename TT,			       \
	    typename...Ts>
  
  /// To be used as an argument of the specialization
#define SFINAE_TEMPLATE_CLASS_SPECIALIZATION_ARG(TYPE)	\
  EnableIfIs ## TYPE<TT<Ts...>,TT<Ts...>>
  
  /// Provides the type inside the specialized class
#define SFINAE_TEMPLATE_CLASS_SPECIALIZATION_PROVIDE_TYPE	       \
  /*! Type used as an argument of the template specialization */       \
  using T=							       \
      TT<Ts...>
}

#endif
