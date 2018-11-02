#ifndef _NNARYSMET_HPP
#define _NNARYSMET_HPP

/// \file NnarySmET.hpp
///
/// \brief Header file defining basic properties of Nnary SmET

#include <ints/IntSeqCat.hpp>
#include <smet/BaseSmET.hpp>
#include <tens/TensKind.hpp>
#include <tuple/TupleOrder.hpp>
#include <utility/Position.hpp>

namespace SUNphi
{
  /// Defines the NnarySmET type traits
  DEFINE_BASE_TYPE(NnarySmET,: public BaseSmET);
  
  // Defines the check for a member type "refs"
  DEFINE_HAS_MEMBER(refs);
  
  /// Defines the check for a Nnary SmET
#define STATIC_ASSERT_IS_NNARY_SMET(...)		\
  STATIC_ASSERT_IS_SMET(__VA_ARGS__);			\
  STATIC_ASSERT_HAS_MEMBER(refs,__VA_ARGS__)
  
  /// Nnary SmET
  template <typename T>  /// Inheriting type
  struct NnarySmET :
    public SmET<T>,
    public BaseNnarySmET
  {
    IS_ASSIGNABLE_ATTRIBUTE(/*! This SmET can never be lhs */,false);
    
    PROVIDE_CRTP_CAST_OPERATOR(T);
  };
  
  /// Provide the references to the objects
  ///
  /// The reference types are contained in a \c Tuple with types
  /// obtained from template pars
#define PROVIDE_NNARY_SMET_REFS_AND_CHECK_ARE_N(N)	\
  /*! Type of the binding references */			\
  using Refs=						\
    Tuple<_Refs...>;					\
  							\
  /*! Reference to the N object */			\
  Tuple<Reference<_Refs>...> refs;			\
  							\
  /*! Type of I-th \c SmET      */			\
  template <int I>					\
  using Ref=						\
    TupleElementType<I,Tuple<_Refs...>>;		\
							\
  /*! Number or \c SmET to act upon */			\
  static constexpr					\
  int NSmET=						\
    N;							\
							\
  /* Check that NSmET coincides with the par */		\
  static_assert(N==sizeof...(_Refs))
  
  /// Returns the size of a \c TensComp, with a simple approach
  ///
  /// Search the asked \c TensComp in each ref \c TensKind iteratively
  /// and returns the size in the first found ref
#define PROVIDE_SIMPLE_NNARY_COMP_SIZE					\
  /*! Returns the size in the first found ref */			\
  template <typename TC, /* \c TensComp to search           */		\
	    int I=0>     /* Integer of the ref to search in */		\
  int compSize() const							\
  {									\
    static_assert(I>=0 and I<NSmET,"Cannot search in negative or larger-than-N ref"); \
									\
    /*! \c TensKind of the I-th \c SmET */				\
    using RefTk=							\
      typename Ref<I>::Tk;						\
									\
    /*! Check if the \c TensKind contains the \c TensComp asked */	\
    constexpr bool found=						\
      RefTk::template contains<TC>;					\
									\
    /* Returns the size if it's in the I-th Tk */			\
    if constexpr(found)							\
		  return get<I>(refs).template compSize<TC>();		\
    else								\
      return compSize<TC,I+1>();					\
  }									\
  SWALLOW_SEMICOLON_AT_CLASS_SCOPE
  
  /// Proivde the position of result Tk \c TensComp in each input
#define PROVIDE_POS_OF_RES_TCS_IN_REFS					\
  /*! Position of all the Result \c TensComp in each \c Refs Tk */	\
  using posOfResTcsInRefsTk=						\
    posOfTcsOfTkInListOfTks<typename Tk::types,RemRef<_Refs>::Tk::types...>
  
  /////////////////////////////////////////////////////////////////
  
  /// Defines a simple creator taking n references
#define PROVIDE_NNARY_SMET_SIMPLE_CREATOR(NNARY_SMET /*!< Name of the NnarySmET */) \
  /*! Constructor taking universal reference */				\
  template <typename...SMETS,						\
	    typename=EnableIf<((isSame<Unqualified<SMETS>,Unqualified<_Refs>>) && ...)>> \
  explicit NNARY_SMET(SMETS&&...smets) : refs(forw<SMETS>(smets)...)	\
  {									\
  }									\
  SWALLOW_SEMICOLON_AT_CLASS_SCOPE
  
  /////////////////////////////////////////////////////////////////
  
  /// Create a simple builder with a name and a NNARY_SMET returned type
  ///
  /// \c NnarySmET cannot use SFINAE to worsen default constructors
  /// (unless packing args in \c Tuple) so the simple builder cannot
  /// be overloaded. If required, use a custom builder and use if
  /// constexpr construct inside it to route the manipulations.
#define SIMPLE_NNARY_SMET_BUILDER(BUILDER,     /*!< Name of builder function        */ \
				  NNARY_SMET)  /*!< Name of the NnarySmET to build */ \
  /*! Simple NNARY_SMET builder called BUILDER */			\
  /*!                                          */			\
  /*! Plain NNARY_SMET getting n plain SmET    */			\
  template <typename...Ts> 	   /* Types of the SmET to get       */ \
  NNARY_SMET<Ts...> BUILDER(Ts&&...smets)    /*!< SmETs to act upon  */ \
  {									\
    return NNARY_SMET<Ts...>(forw<Ts>(smets)...);			\
  }									\
  SWALLOW_SEMICOLON_AT_GLOBAL_SCOPE
  
  /// Set aliasing according to the isAliasing of references
  /// \todo enforce cehck only with TensClass
#define FORWARD_IS_ALIASING_TO_REFS			\
  /*! Forward aliasing check to the references */	\
  template <typename Tref>				\
  bool isAliasing(const Tref& alias) const		\
  {							\
    CRASH("Fixme");					\
    return true;					\
  }							\
  SWALLOW_SEMICOLON_AT_CLASS_SCOPE
}

#endif
