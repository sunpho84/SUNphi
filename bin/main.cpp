//#define DEBUG_TENS
//#define DEBUG_BINDER
//#define DEBUG_INDEXER
//#define DEBUG_REL_BINDER

#include <SUNphi.hpp>

using namespace std;
using namespace SUNphi;

/// Check that \c conj is ignored when Compl is not a component
///
/// Check that \c conj of a non-complex type object is the same of
/// original type
void checkNonComplConjCancelation()
{
  /// Tensor Kind to be used
  using MyTk=
    TensKind<RwCol,
	     Spin,
	     CnCol>;
  
  /// Tensor class to be used
  using MyTens=
    Tens<MyTk,
	 double>;
  
  /// Tensor instance
  MyTens c;
  
  /// Result of conj
  using Res=
    decltype(conj(c));
  
  // Check that the original type is the same of the result
  STATIC_ASSERT_IS_BASE_OF(MyTens,Res);
}

/// Check nested \c conj cancelation
///
/// Check that \c conj of a complex type object is the same of original
/// type, by issuing twice \c conj on a Tensor
void checkNestedConjCancelation()
{
  /// Tensor Kind to be used
  using MyTk=
    TensKind<RwCol,
	     Compl,
	     CnCol>;
  
  /// Tensor class to be used
  using MyTens=
    Tens<MyTk,
	 double>;
  
  /// Tensor instance
  MyTens c;
  
  /// Result of double conj
  using Res=
    decltype(conj(conj(c)));
  
  // Check that the original type is the same of the result
  STATIC_ASSERT_IS_BASE_OF(MyTens,Res);
}

/// Check duplicated "-" cancellation
///
/// Check that repeated minus is canceled, by issuing twice it ovr a tensor
void checkDuplicatedUMinusCancellation()
{
  /// Tensor Kind to be used
  using MyTk=
    TensKind<RwCol,
	     Spin,
	     CnCol>;
  
  /// Tensor class to be used
  using MyTens=
    Tens<MyTk,
	 double>;
  
  /// Tensor instance
  MyTens c;
  
  /// Result of double -
  using Res=
    decltype(-(-c));
  
  // Check that the original type is the same of the result
  STATIC_ASSERT_IS_BASE_OF(MyTens,Res);
}

/// Check ability to filter the position of the variadic class
///
/// First a variadic class, specifiied by integer of various
/// value. Then, a filter through "IsNotNull" is issued. Lastly the
/// type resulting is checked
void checkFilterVariadicClassPos()
{
  /// Variadic class to be filtered
  using VClass=
    IntSeq<0,1,0,10>;
  
  /// Result of the filtering
  using Res=
    FilterVariadicClassPos<IsNotNull,
			   VClass>;
  
  /// Comparison of the fileer
  using Ref=IntSeq<1,3>;
  
  // Check
  STATIC_ASSERT_IS_BASE_OF(Res,Ref);
}

/// Check the sum of two \c SmET
///
/// Two tensors are tried to be summed
void checkSumOfTwoSmETs()
{
  /// Tensor kind to be used
  using MyTk=
    TensKind<RwCol,
	     Spin>;
  
  /// Tensor class to be used
  using MyTens=
    Tens<MyTk,
	 double>;
  
  /// Tensors used to test the sum
  MyTens a,b;
  
  //c=
  a+b;
}


//
int main()
{
  checkNonComplConjCancelation();
  
  checkNestedConjCancelation();
  
  checkFilterVariadicClassPos();
  
  checkSumOfTwoSmETs();
  
  // Check on Diag
  {
    using MyTk=TensKind<RwCol,Spin,CnCol>;
    using A=IntSeq<1,0,1>;
    using B=MyTk::IsMatrixComp;
    STATIC_ASSERT_IS_BASE_OF(A,B);
    
    using F=MyTk::Diag;
    
    
    
    using G=TensKind<RwCol,Spin>;
    STATIC_ASSERT_IS_BASE_OF(G,F);
    
  }
  
  // Check relBind
  {
    using MyTk=TensKind<RwCol,Spin,Spacetime,CnCol>;
    
    using MyTens=Tens<MyTk,double>;
    MyTens c(10);
    
    c.eval(1,2,0,1)=1.0594;
    
    auto b1=relBind<RwCol,CnCol>(c,[](int id){return id;});
    auto b2=relBind<CnCol,RwCol>(c,[](int id){return id;});
    // cout<<decltype(c)::Tk::name()<<endl;
    // cout<<decltype(b1)::Tk::name()<<endl;
    // cout<<decltype(b2)::Tk::name()<<endl;
    cout<<c.eval(1,2,0,1)<< " "<<b1.eval(2,0,1)<<" "<<b2.eval(1,2,0)<<endl;
    
    auto bb1=relBind<Spin,Col>(b1,[](int id){return id+1;});
    auto bb2=relBind<Spin,Col>(b2,[](int id){return id+1;});
    
    cout<<c.eval(1,2,0,1)<< " "<<bb1.eval(0,1)<<" "<<bb2.eval(1,0)<<endl;
    
    cout<<decltype(c.getMaximallyMergedCompsView())::Tk::name()<<endl;
    cout<<" c: "<<decltype(c)::Tk::name()<<endl;
    cout<<" bb2: "<<decltype(bb2)::Tk::name()<<endl;
    cout<<decltype(b1.getMaximallyMergedCompsView())::Tk::name()<<endl;
  }
  
  //come trovare le componenti non twinnate? crea un IntSeq che dica
  //se le componenti devono essere prese o no, prendi di questo le
  //posizioni non nulle, e chiama getIndexed
  {
    using MyTk=TensKind<RwCol,Spin,CnCol>;
    using MyTens=Tens<MyTk,double>;
    
    MyTens c;
    
    cout<<" "<<posOfType<RwCol,MyTk::Twinned::types><<endl;
    
    STATIC_ASSERT_IS_BASE_OF(MyTens,decltype(+c));
  }
  
  //check the blending of two TensKind
  {
    using MyTk1=TensKind<RwCol,Spin>;
    using MyTk2=TensKind<RwCol,CnCol>;
    using MyTkBCompa=TensKind<RwCol,Spin,CnCol>;
    
    using MyTkBRes1=typename MyTk1::BlendWithTensKind<MyTk2>;
    STATIC_ASSERT_IS_BASE_OF(MyTkBRes1,MyTkBCompa);
    
    using MyTkBRes2=BlendTensKinds<MyTk1,MyTk2>;
    STATIC_ASSERT_IS_BASE_OF(MyTkBRes2,MyTkBCompa);
    
    using MyTkBRes3=BlendTensKinds<MyTk1,MyTk2,MyTk2>;
    STATIC_ASSERT_IS_BASE_OF(MyTkBRes3,MyTkBCompa);
  }
  
  
  using MyTk=TensKind<RwCol,Spin,Compl,CnCol>;
  using MyTens=Tens<MyTk,double>;
  
  MyTens cicc;
  for(int irw_col=0;irw_col<NCOL;irw_col++)
    for(int ispin=0;ispin<NSPIN;ispin++)
      for(int icn_col=0;icn_col<NCOL;icn_col++)
	for(int ri=0;ri<NCOMPL;ri++)
	  rwCol(reim(cnCol(spin(cicc,
				ispin),
			   icn_col),
		     ri),
		irw_col)=
  	  icn_col+NCOL*(0+NCOMPL*(ispin+NSPIN*irw_col));

  cicc.getMaximallyMergedCompsView()=1.0;
  
  auto i=imag(conj(cnCol(spin(rwCol(-cicc,0),0),0)));
  cout<<i<<endl;
  
  cout<<"/////////////////////////////////////////////////////////////////"<<endl;
  
  for(int irw_col=0;irw_col<NCOL;irw_col++)
    for(int ispin=0;ispin<NSPIN;ispin++)
      for(int icn_col=0;icn_col<NCOL;icn_col++)
	for(int ri=0;ri<NCOMPL;ri++)
	  {
	    cout<<"------"<<endl;
	    cout<<&rwCol(reim(cnCol(spin(cicc,
					 ispin),
				  icn_col),
			      ri),
			 irw_col)<<endl;
	  }
  
  cout<<"/////////////////////////////////////////////////////////////////"<<endl;
  
  double a=0.0;
  for(int irw_col=0;irw_col<NCOL;irw_col++)
    for(int ispin=0;ispin<NSPIN;ispin++)
      for(int icn_col=0;icn_col<NCOL;icn_col++)
	a+=imag(conj(rwCol(cnCol(spin(cicc,
				 ispin),
			    icn_col),
			   irw_col)));
	// a+=real(conj(rwCol(cnCol(spin(cicc,
	// 			 ispin),
	// 		    icn_col),
	// 		   irw_col)));
  
  cout<<a<<endl;
  
  return 0;
}
