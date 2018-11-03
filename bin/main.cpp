//#define DEBUG_TENS
//#define DEBUG_BINDER
//#define DEBUG_INDEXER
//#define DEBUG_REL_BINDER

#include <SUNphi.hpp>

#include <iostream>

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

/// Check that a simple type can be wrapped into a ScalarWrapper
void checkScalarWrap()
{
  /// Simple type
  int a=
    10;
  
  /// Wrap into a SmeT
  auto b=
    scalarWrap(a);
  
  cout<<"scalarWrap(int)"<<
    ", TensKind: "<<decltype(b)::Tk::name()<<
    ", val: "<<b.eval()<<
    ", type id: "<<typeid(decltype(b)).name()<<endl;
  
  /// Rewrap into a SmeT
  auto c=
    scalarWrap(b);
  
  // Check that b and c are of the same type
  STATIC_ASSERT_IS_BASE_OF(decltype(c),decltype(b));
  
  // Write some info
  cout<<"scalarWrap(scalarWrap(int))"<<
    ", TensKind: "<<decltype(c)::Tk::name()<<
    ", val: "<<c.eval()<<
    ", type id: "<<typeid(decltype(c)).name()<<endl;
  
  /// Rewrap into a SmeT an rvalue
  auto d=
    scalarWrap(10);
  
  // Write some info
  cout<<"scalarWrap(10)"<<
    ", TensKind: "<<decltype(d)::Tk::name()<<
    ", val: "<<d.eval()<<
    ", type id: "<<typeid(decltype(d)).name()<<endl;
}

/// Checks that a TensKind is contained into another one
void checkTensKindIscontained()
{
  /// Fist Tensor Kind to be used
  using MyTk1=
    TensKind<RwCol,
	     Spin,
	     CnCol>;
  
  /// Second Tensor Kind to be used
  using MyTk2=
    TensKind<RwCol,
	     CnCol>;
  
  static_assert(MyTk1::contains<MyTk2>,"First TensKind not contained");
  static_assert(not MyTk2::contains<MyTk1>,"Second TensKind contained");
}

/// Check the implementation of the call operator
void checkCallOperator()
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
  
  /// Tensor to be used
  MyTens tens;
  
  /// Value to be assigned
  const double val=
    10.0;
  
  // Try to assign and check it
  tens=val;
  
  /// Result of the check
  auto res=
    tens(0,0,0);
  
  /// Type of the result, to be checked
  using D=
    decltype(res);
  
  cout
    <<"Call result: "<<res<<", "
    <<"is_lvalue_reference: "<<std::is_lvalue_reference_v<D><<", "
    <<"is_const: "<<std::is_const_v<decltype(tens(0,0,0))>
    <<endl;
}

#include <sstream>

/// Prints an IntSeq
template <int...Ints>
std::string printIntSeq(IntSeq<Ints...>)
{
  std::ostringstream os;
  for(int a : {Ints...})  os<<a<<" ";
  return os.str();
}

void checkNnaryBuilder()
{
  /// Fist Tensor Kind to be used
  using MyTk1=
    TensKind<RwCol,
	     Spin,
	     CnCol>;
  
  /// Second Tensor Kind to be used
  using MyTk2=
    TensKind<RwCol,
	     CnCol>;
  
  /// Third Tensor Kind to be used
  using MyTk3=
    TensKind<Spin,
	     CnCol>;
  
  /// First Tensor class to be used
  using MyTens1=
    Tens<MyTk1,
	 double>;
  
  /// Second Tensor class to be used
  using MyTens2=
    Tens<MyTk2,
	 float>;
  
  /// Second Tensor class to be used
  using MyTens3=
    Tens<MyTk3,
	 float>;
  
  /// First Tensor to be used
  MyTens1 tens1;
  
  /// Second Tensor to be used
  MyTens2 tens2;
  
  /// Third Tensor to be used
  MyTens3 tens3;
  
  [[ maybe_unused ]]
  auto mulAdder=
    mulAdd(tens3,tens2,tens1);
  
  using Mu=
    decltype(mulAdder);
  
  using Pos=
    Mu::PosOfResTcsInRefsTk;
  
  using Md=
    Mu::MergingDelimsForRefs<IntSeq<0,3>>;
  
  //int y=CompsMergeability<Md,Pos>{};
}

/// Check the sum of two \c SmET
///
/// Two tensors are tried to be summed
void checkSumOfTwoSmETs()
{
  /// Tensor kind to be used for first SmET
  using MyTk1=
    TensKind<CnCol,
	     RwCol,
	     CnSpin>;
  
  /// Tensor class to be used for first tensor
  using MyTens1=
    Tens<MyTk1,
	 double>;
  
  /// Tensor kind to be used for second SmET
  using MyTk2=
    TensKind<RwCol,
	     // RwSpin,
	     CnSpin,
	     Compl>;
  
  /// Tensor class to be used for second tensor
  using MyTens2=
    Tens<MyTk2,
	 double>;
  
  /// Tensors used to test the sum
  MyTens1 a;
  for(int iCnCol=0;iCnCol<NCOL;iCnCol++)
    for(int iRwCol=0;iRwCol<NCOL;iRwCol++)
      for(int iCnSpin=0;iCnSpin<NSPIN;iCnSpin++)
	cnCol(rwCol(cnSpin(a,iCnSpin),iRwCol),iCnCol)=3.0;
  
  MyTens2 b;
  for(int iRwCol=0;iRwCol<NCOL;iRwCol++)
    for(int iCnSpin=0;iCnSpin<NSPIN;iCnSpin++)
      for(int iReIm=0;iReIm<NCOMPL;iReIm++)
	rwCol(cnSpin(reIm(b,iReIm),iCnSpin),iRwCol)=5.0;
  
  Tens<decltype((a+b).getMaximallyMergedCompsView())::Tk,double> d;
  cout<<"/////////////////////////////////////////////////////////////////"<<endl;
  d=(a+b).getMaximallyMergedCompsView();
  cout<<"/////////////////////////////////////////////////////////////////"<<endl;
  
  cout<<"d: "<<d.eval(1,1,1)<<endl;
  
  auto sum=a+b;
  // this was added here to show the maximally mergeable delims
  //int e=decltype(sum)::MergeableComps{};
  //int e=decltype(sum)::PosOfRef1TcsInResTk{};
  //int e=decltype(sum)::MergedDelims1<IntSeq<0,1,3,4>>{};
  
  
  //cout<<"Col: "<<c.compSize<RwCol>()<<endl;
  //cout<<"Spin: "<<c.compSize<CnSpin>()<<endl;
  cout<<"Tensor Kind of addendum 1: "<<decltype(a)::Tk::name()<<endl;
  using M1=
    typename decltype(a)::MergeableComps;
  cout<<"Mergeability of addendum 1: "<<printIntSeq(M1{})<<endl;
  cout<<"Tensor Kind of addendum 2: "<<decltype(b)::Tk::name()<<endl;
  using M2=
    typename decltype(b)::MergeableComps;
  cout<<"Mergeability of addendum 2: "<<printIntSeq(M2{})<<endl;
  cout<<"Tensor Kind: "<<decltype(a+b)::Tk::name()<<endl;
  
  cout<<"PosOf types 1 present: "<<printIntSeq(decltype(a+b)::PosOfRef1PresTcsInResTk{})<<endl;
  cout<<"PosOf types 2 present: "<<printIntSeq(decltype(a+b)::PosOfRef2PresTcsInResTk{})<<endl;
  // using P1=
  //   decltype(c)::posOfAddend1TcInResTk;
  // cout<<"PosOf types 1: "<<printIntSeq(P1{})<<endl;
  // using P2=
  //   decltype(c)::posOfAddend2TcInResTk;
  // cout<<"PosOf types 2: "<<printIntSeq(P2{})<<endl;
  
  // using P3=
  //   PairOfTensKindMergeability::template CompsMergeability<M1,M2,P1,P2>;
  
  // cout<<"Mergeability: "<<printIntSeq(P3{})<<endl;
  
  // using MDel=
  //   IntSeq<0,1,3,4>;
  
  // cout<<"Trying to merge with: "<<printIntSeq(MDel{})<<endl;
  
  // using P4=
  //   decltype(c)::template MergedDelims1<MDel>;
  
  // cout<<"The first needs to be merged like this: "<<printIntSeq(P4{})<<endl;
  
}


//
int main()
{
  checkNonComplConjCancelation();
  
  checkNestedConjCancelation();
  
  checkFilterVariadicClassPos();
  
  //checkSumOfTwoSmETs();
  
  checkScalarWrap();
  
  //checkCallOperator();

  using U=IntSeqsTranspose<Tuple<IntSeq<0,2>,IntSeq<1,3>>>;
  
  cout<<printIntSeq(TupleElementType<1,U>{})<<endl;
  
  return 0;
  
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
	  rwCol(reIm(cnCol(spin(cicc,
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
	    cout<<&rwCol(reIm(cnCol(spin(cicc,
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
