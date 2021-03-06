#ifndef _TENSCLASS_HPP
#define _TENSCLASS_HPP

/// \file TensClass.hpp
///
/// \brief Header file for the Tens class

#include <ios/Logger.hpp>
#include <metaprogramming/SFINAE.hpp>
#include <tens/TensKind.hpp>
#include <tens/TensStor.hpp>
#include <smet/Assign.hpp>
#include <smet/NnarySmET.hpp>

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
	    typename FUND> // Fundamental type
  class Tens :
    public BaseTens,                       // Inherit from BaseTens to detect in expression
    public NnarySmET<Tens<TK,FUND>>,       // Inherit from NnarySmET
    public ConstrainIsTensKind<TK>,        // Constrain the TK type to be a TensKind
    public ConstrainIsFloatingPoint<FUND>  // Constrain the Fund type to be a floating point
  {
  public:
    
    /// Tensor kind of the tensor
    PROVIDE_TK(TK);
    
    /// Tensor fundamental type of the tensor
    PROVIDE_FUND(FUND);
    
    // Attributes
    ASSIGNABLE;
    STORING;
    IS_ALIASING_ACCORDING_TO_POINTER(v);
    
  private:
    
    /// Internal storage, inited to null
    TensStor<Tk,Fund>* v=
      nullptr;
    
    /// Keep note of whether we need to free at destroy
    bool freeAtDestroy;
    
  public:
    
    PROVIDE_MERGEABLE_COMPS(/* By default, all components can be merged */,IntSeq<0,Tk::nTypes>);
    
    /// Returns the size of a component
    template <typename TC>  // Name of the component
    int compSize() const
    {
      return v->template compSize<TC>();
    }
    
    /// Construct the Tens on the basis of the dynamical sizes passed
    template <class...DynSizes,                                               //   Dynamic size types
	      typename=EnableIf<areIntegrals<Unqualified<DynSizes>...>>>      //   Constrain to be integers
    explicit Tens(DynSizes&&...extDynSizes) :                    ///< Passed internal dynamic size
      v(new TensStor<Tk,Fund>(forw<DynSizes>(extDynSizes)...)),  //   Construct the vector
      freeAtDestroy(true)                                        //   We are allocating, so we need to free
    {
#ifdef DEBUG_TENS
      using namespace std;
      cout<<"TensClass alloc: "<<this<<endl;
#endif
      STATIC_ASSERT_ARE_N_TYPES(Tk::nDynamic,DynSizes);
    }
    
    /// Construct the Tens on the basis of a reference storage
    explicit Tens(TensStor<Tk,Fund>* v) :                        ///< Provided storage
      v(v),                       // The internal storage is built with the reference
      freeAtDestroy(false)        // We are not allocating, so we need not to free
    {
#ifdef DEBUG_TENS
      using namespace std;
      cout<<"Creating a Tens of type "<<Tk::name()<<" NOT allocating"<<endl;
#endif
    }
    
//     /// Copy constructor
//     Tens(const Tens& oth) :
//       v(new TensStor<Tk,Fund>(*oth.v)),   //   Copy the vector
//       freeAtDestroy(true)                 //   We are allocating, so we need
//     {
//       printf("Copying a Tens of type %s NOT allocating\n",Tk::name());
// #ifdef DEBUG_TENS
//       using namespace std;
//       cout<<"Tens copy constructor!"<<endl;
// #endif
//     }
    
    /// Move constructor
//     Tens(Tens&& oth) : freeAtDestroy(oth.freeAtDestroy) // Deallocate according to arg
//     {
//       oth.freeAtDestroy=false;
      
//       v=oth.v;
//       oth.v=nullptr;
      
//       printf("Creating a Tens of type %s moving\n",Tk::name());
      
// #ifdef DEBUG_TENS
//       using namespace std;
//       cout<<"Tens move constructor!"<<endl;
// #endif
//     }
    
    /// Destructor
    ~Tens()
    {
#ifdef DEBUG_TENS
      using namespace std;
      cout<<"TensClass destroy: "<<this<<endl;
#endif
      
      if(freeAtDestroy)
	{
#ifdef DEBUG_TENS
 	  printf("Destroying a Tens of type %s deallocating\n",Tk::name());
#endif
	  delete v;
	}
#ifdef DEBUG_TENS
      else
	{
	  printf("Destroying a Tens of type %s NOT deallocating\n",Tk::name());
	}
#endif
    }
    
  public:
    
    /// Returns a component-merged version
    PROVIDE_GET_MERGED_COMPS_VIEW(/*! Create a reference to the same storage with appropriate DynSizes */,
				  /* Get a component-merged reference to the storage */
				  auto vMerged=
				    v->template mergedComps<Is>();
				  /* Returned TensKind */
				  using MergedTk=
				    typename Unqualified<decltype(*vMerged)>::Tk;
				  /* Returned type */
				  using TOut=
				    Tens<MergedTk,Fund>;
				  
				  return TOut(vMerged));
    
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
      if(DEBUG_TENS_COMPONENTS)						\
	((runLog()<<"Components: "<<&v) * ... *comps);			\
									\
      return								\
	v->eval(forw<const Comps>(comps)...);				\
    }
    
    PROVIDE_EVALUATOR(NON_CONST_QUALIF);
    PROVIDE_EVALUATOR(CONST_QUALIF);
    
#undef PROVIDE_EVALUATOR
    
    PROVIDE_SMET_ASSIGNEMENT_OPERATOR(Tens);
  };
  
  // Check that a test \c Tens is a \c NnarySmET
  STATIC_ASSERT_IS_SMET(Tens<TensKind<TensComp<double,1>>,double>);
  
  /// Get the storage of a \c SmET
  ///
  /// default case, asserting if smet is not a \c NnarySmET as needed
  template <typename SMET,      // Type of the \c SmET
	    SFINAE_WORSEN_DEFAULT_VERSION_TEMPLATE_PARS>
  void getStor(SMET&& smet,      ///< \c SmET to be searched
	       SFINAE_WORSEN_DEFAULT_VERSION_ARGS)
  {
    SFINAE_WORSEN_DEFAULT_VERSION_ARGS_CHECK;
    STATIC_ASSERT_IS_NNARY_SMET(SMET);
  }
  
  /// Get the storage of a \c SmET
  ///
  /// In this case we are treating a \c NnarySmET which is not a Tens
  template <typename SMET,     // Type of the SmET
	    SFINAE_ON_TEMPLATE_ARG(isNnarySmET<SMET> and not isTens<SMET>)>
  DECLAUTO getStor(SMET&& smet)    ///< \c SmET to be searched
  {
    return getStor(get<0>(smet.refs));
  }
  
  /// Get the storage of a SmET
  ///
  /// In this case we are treating a UnarySmET which is a Tens
  template <typename SMET,     // Type of the SmET
	    SFINAE_ON_TEMPLATE_ARG(isNnarySmET<SMET> and isTens<SMET>)>
  auto& getStor(SMET&& smet)    ///< SmET to be searched
  {
    return smet.getStor();
  }
}

#endif
