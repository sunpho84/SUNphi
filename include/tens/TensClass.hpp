#ifndef _TENSCLASS_HPP
#define _TENSCLASS_HPP

/// \file TensClass.hpp
///
/// \brief Header file for the Tens class

#include <metaprogramming/SFINAE.hpp>
#include <tens/TensKind.hpp>
#include <tens/TensStor.hpp>
#include <tex/UnaryTEx.hpp>

namespace SUNphi
{
  // Base type to qualify as Tens
  DEFINE_BASE_TYPE(Tens);
  
  /// A Tensor class
  ///
  /// Container with a given TensKind structure and fundamental type,
  /// holding resources for the storage of the data and providing
  /// evaluator
  template <typename TK,   // List of tensor components
	    typename Fund> // Fundamental type
  class Tens :
    public BaseTens,                       // Inherit from BaseTens to detect in expression
    public UnaryTEx<Tens<TK,Fund>>,        // Inherit from UnaryTEx
    public ConstrainIsTensKind<TK>,        // Constrain the TK type to be a TensKind
    public ConstrainIsFloatingPoint<Fund>  // Constrain the Fund type to be a floating point
  {
    
  public:
    
    /// Tensor kind of the tensor
    using Tk=TK;
    
    // Attributes
    ASSIGNABLE;
    STORING;
    IS_ALIASING_ACCORDING_TO_POINTER(v);
    
  private:
    
    /// Internal storage
    TensStor<Tk,Fund>* v=nullptr;
    
    /// Store whether this has been created
    bool created=false;
    
  public:
    
    PROVIDE_MERGEABLE_COMPS(/* By default, all components can be merged */,IntSeq<0,Tk::nTypes>);
    
    /// Returns the size of a component
    template <typename TC>
    int compSize() const
    {
      return v->template compSize<TC>();
    }
    
    /// Construct the Tens on the basis of the dynamical sizes passed
    template <class...DynSizes,                                               //   Dynamic size types
	      typename=EnableIf<areIntegrals<Unqualified<DynSizes>...>>>      //   Constrain to be integers
    explicit Tens(DynSizes&&...extDynSizes) :                    ///< Passed internal dynamic size
      v(new TensStor<Tk,Fund>(forw<DynSizes>(extDynSizes)...)),  //   Construct the vector
      created(true)                                              //   Mark down that we are creating
    {
#ifdef DEBUG_TENS
	  using namespace std;
	  cout<<"TensClass alloc: "<<this<<endl;
#endif
      STATIC_ASSERT_ARE_N_TYPES(Tk::nDynamic,DynSizes);
    }
    
    /// Copy constructor
    Tens(const Tens& oth) :
      v(new TensStor<Tk,Fund>(*oth.v)),   //   Copy the vector
      created(true)
    {
#ifdef DEBUG_TENS
      using namespace std;
      cout<<"Tens copy constructor!"<<endl;
#endif
    }
    
    /// Move constructor
    Tens(Tens&& oth)
    {
      v=oth.v;
      oth.v=nullptr;
      
      created=oth.created;
      oth.created=false;
      
#ifdef DEBUG_TENS
      using namespace std;
      cout<<"Tens move constructor!"<<endl;
#endif
    }
    
    /// Destructor
    ~Tens()
    {
#ifdef DEBUG_TENS
      using namespace std;
      cout<<"TensClass dealloc: "<<this<<endl;
#endif
      if(created)
	delete v;
    }
    
  public:
    
    /// Returns a constant reference to v
    const TensStor<Tk,Fund>& getStor() const
    {
      return *v;
    }
    
    /// Returns a non-constant reference to v
    TensStor<Tk,Fund>& getStor()
    {
      return *v;
    }
    
    /// Enable or not printing the components
#ifdef DEBUG_TENS
    #define DEBUG_TENS_COMPONENTS 1
#else
    #define DEBUG_TENS_COMPONENTS 0
#endif
    
    /// Provides either the const or non-const evaluator
#define PROVIDE_EVALUATOR(QUALIFIER)					\
    /*! Evaluate the tensor (returns QUALIFIED reference to internal data) */ \
    template <class...Comps,                              /* Component types                                        */ \
	      class=ConstrainAreIntegrals<Comps...>,      /* Force the components to be integer-like                */ \
	      class=ConstrainNTypes<Tk::nTypes,Comps...>> /* Constrain the component to be in the same number of Tk */ \
    QUALIFIER Fund& eval(const Comps&...comps) QUALIFIER  /*!< Component values                                     */ \
    {									\
      if(DEBUG_TENS_COMPONENTS) print(std::cout,"Components: ",&v,comps...,"\n");	\
      return v->eval(forw<const Comps>(comps)...);			\
    }									\
    SWALLOW_SEMICOLON_AT_CLASS_SCOPE
    
    PROVIDE_EVALUATOR(NON_CONST_QUALIF);
    PROVIDE_EVALUATOR(CONST_QUALIF);
    
#undef PROVIDE_EVALUATOR
    
    PROVIDE_UNARY_TEX_ASSIGNEMENT_OPERATOR(Tens);
    
  };
  
  // Check that a test Tens is a UnaryTEx
  STATIC_ASSERT_IS_TEX(Tens<TensKind<TensComp<double,1>>,double>);
  
  /// Get the storage of a TEx
  ///
  /// default case, asserting if tex is not a UnaryTEx as needed
  template <typename TEX,      // Type of the TEx
	    SFINAE_WORSEN_DEFAULT_VERSION_TEMPLATE_PARS>
  void getStor(TEX&& tex,      ///< TEx to be searched
	       SFINAE_WORSEN_DEFAULT_VERSION_ARGS)
  {
    SFINAE_WORSEN_DEFAULT_VERSION_ARGS_CHECK;
    STATIC_ASSERT_IS_UNARY_TEX(TEX);
  }
  
  /// Get the storage of a TEx
  ///
  /// In this case we are treating a UnaryTEx which is not a Tens
  template <typename TEX,     // Type of the TEx
	    SFINAE_ON_TEMPLATE_ARG(isUnaryTEx<TEX> and not isTens<TEX>)>
  auto& getStor(TEX&& tex)    ///< TEx to be searched
  {
    return getStor(tex.ref);
  }
  
  /// Get the storage of a TEx
  ///
  /// In this case we are treating a UnaryTEx which is a Tens
  template <typename TEX,     // Type of the TEx
	    SFINAE_ON_TEMPLATE_ARG(isUnaryTEx<TEX> and isTens<TEX>)>
  auto& getStor(TEX&& tex)    ///< TEx to be searched
  {
    return tex.getStor();
  }
}

#endif
