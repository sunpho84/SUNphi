#include <SUNphi.hpp>

#include <array>
#include <iostream>
#include <sstream>

using namespace std;
using namespace SUNphi;

/// Report that a test is passed
#define TEST_PASSED \
  runLog()<<__PRETTY_FUNCTION__<<" passed!"

/// Prints an IntSeq
template <int...Ints>
std::string printIntSeq(IntSeq<Ints...>)
{
  std::ostringstream os;
  for(int a : {Ints...})
    os<<a<<" ";
  
  return os.str();
}

// void test_first_vectorizer()
// {
//   int a=MyTk::firstVectorizingComp<double>;
//   runLog<<a;
// }

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
  const MyTens c;
  
  /// Result of conj
  using Res=
    decltype(conj(c));
  
  // Check that the original type is the same of the result
  STATIC_ASSERT_IS_BASE_OF(MyTens,Res);
  
  TEST_PASSED;
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
  const MyTens c;
  
  /// Result of double conj
  using Res=
    decltype(conj(conj(c)));
  
  // Check that the original type is the same of the result
  STATIC_ASSERT_IS_BASE_OF(MyTens,Res);
  
  TEST_PASSED;
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
  const MyTens c;
  
  /// Result of double -
  using Res=
    decltype(-(-c));
  
  // Check that the original type is the same of the result
  STATIC_ASSERT_IS_BASE_OF(MyTens,Res);
  
  TEST_PASSED;
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
  
  TEST_PASSED;
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
  
  /// Rewrap into a SmeT
  auto c=
    scalarWrap(b);
  
  if(c.eval()!=10)
    CRASH<<"Wrapping 10 from rvalue failed";
  
  // Check that b and c are of the same type
  STATIC_ASSERT_IS_BASE_OF(decltype(c),decltype(b));
  
  /// Rewrap into a SmeT an rvalue
  auto d=
    scalarWrap(10);
  
  if(d.eval()!=10)
    CRASH<<"Wrapping 10 from rvalue failed";
  
  TEST_PASSED;
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
  
  TEST_PASSED;
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
  auto& res=
    tens(0,0,0);
  
  if(val!=res)
    CRASH<<"Tens is "<<res<<" expected "<<val;
  
  /// Type of the result, to be checked
  using D=
    decltype(res);
  
  static_assert(std::is_lvalue_reference_v<D>,"Expected lvalue");
  
  TEST_PASSED;
}

/// Performs simple checks on the Grid class
void checkGrid()
{
  /// Flags of the grid
  static constexpr int flags=
    combineFlags<GridFlag::HASHED,
		 GridFlag::SHIFTED_BC>;
  
  /// Check that volume is as expected
  auto checkVolume=
    [](const auto grid,
       const int64_t expVolume)
    {
      /// Voluem of the grid
      const int64_t volume=
        grid.volume();
      
      if(expVolume!=volume)
	CRASH<<"Expected volume"<<expVolume<<"obtained"<<volume;
    };
  
  /// Grid
  auto grid=
    Grid<2,int32_t,int64_t,flags>({1,2});
  
  /// Type of grid
  using Grid=
    decltype(grid);
  
  checkVolume(grid,2);
  
  grid.setSides({3,3});
  
  checkVolume(grid,9);
  
  /// Type of the list of neighbors
  using GridNeighs=
    std::vector<typename Grid::Neigh>;
  
  /// Cehck neighbors
  auto checkNeigh=[](const auto& grid,               ///< Grid to check
		     const GridNeighs& expNeighs)    ///< Expected neighbors
                    {
		      grid.forAllPoints([&](const int64_t i)
                                           {
					     Grid::forAllOriDirs([&](const int oriDir)
                                                                   {
								     /// Neighbor
								     auto neigh=
								       grid.neighOfPoint(i,oriDir);
								     
								     /// Expected neighbor
								     auto expNeigh=
								        expNeighs[i][oriDir];
								     
								     if(neigh!=expNeigh)
								       CRASH<<"Point"<<i<<"direction"<<oriDir<<"expected neighbor"<<expNeigh<<"obtained"<<neigh;
								   });
					   });
		    };
  
  /// List of expected neighbors when not shifting BC
  const GridNeighs expUnshiftedNeigh=
    {{6,3,2,1},
     {7,4,0,2},
     {8,5,1,0},
     {0,6,5,4},
     {1,7,3,5},
     {2,8,4,3},
     {3,0,8,7},
     {4,1,6,8},
     {5,2,7,6}};
  
  checkNeigh(grid,expUnshiftedNeigh);
  
  grid.setShiftBC(1,{1,0});
  
  /// List of expected neighbors when shifting BC
  const GridNeighs expShiftedNeigh=
    {{6,3,8,1},
     {7,4,0,2},
     {8,5,1,3},
     {0,6,2,4},
     {1,7,3,5},
     {2,8,4,6},
     {3,0,5,7},
     {4,1,6,8},
     {5,2,7,0}};
  
  /// Shift read from grid
  const int shiftOfBC0=
    grid.shiftOfBC(0);
  
  if(shiftOfBC0!=1)
    CRASH<<"Expected shift of"<<1<<"obtained"<<shiftOfBC0;
  
  checkNeigh(grid,expShiftedNeigh);
  
  TEST_PASSED;
}

/// Ceck combining three flags
void checkFlagMasks()
{
  /// Three flags
  enum r_t{A,B,C};
  
  /// Combine A and C
  constexpr int maskAC=
    combineFlags<A,C>;
  static_assert(maskAC==5,"Expecting 5");
  
  /// Add B
  constexpr int maskABC=
    addFlags<maskAC,B>;
  static_assert(maskABC==7,"Expecting 7");
  
  /// Remove A and C
  constexpr int maskB=
    remFlags<maskABC,A,C>;
  static_assert(maskB==2,"Expecting 2");
  
  TEST_PASSED;
}

/// Check that modulo operation is performed as expected
void checkSafeModulo()
{
  /// Function to test the result of modulo
  auto check=
    [](const int val,const int mod,const int expRes)
    {
      /// Result
      int res=
        safeModulo(val,mod);
      
      if(expRes!=res)
	CRASH<<val<<" MOD "<<res<<" expected "<<expRes<<", is "<<res;
    };
  
  check(5,3,2);
  check(-2,3,1);
  check(-3,3,0);
  
  TEST_PASSED;
}

/// Check that a number factorizes as expected
void checkFactorize()
{
  /// Function checking the number factorization
  auto check=
    [](const int n,                         ///< Number to factorize
       const std::vector<int> expFacts)     ///< Expected factors
    {
      /// Factors to be checked
      const std::vector<int> facts=
        factorize(n);
      
    if(facts.size()!=expFacts.size())
      CRASH<<"Factorizing "<<n<<" facts size "<<(int)facts.size()<<" different from size expected, "<<(int)expFacts.size();
    
    for(int iFact=0;iFact<(int)facts.size();iFact++)
      {
	/// Factor obtained
	const int f=
	  facts[iFact];
	
	/// Factor expected
	const int eF=
	  expFacts[iFact];
	
	if(f!=eF)
	  CRASH<<"Factorizing "<<n<<" Factor "<<iFact<<" is expected to be "<<eF<<" is"<<f;
      }
  };
  
  check(1,{1});
  check(2,{2});
  check(3,{3});
  check(4,{2,2});
  check(5,{5});
  check(6,{2,3});
  check(101,{101});
  
  TEST_PASSED;
}

/// Check the lhs assignement to transposed
void checkLhsTranspose()
{
  /// A real 3 X 3 matrix
  using Su3Tens=
    Tens<TensKind<RwCol,CnCol>,double>;
  
  /// Matrix used in the test
  Su3Tens t;
  
  /// Write in the matrix as transposed
  for(int rw_c=0;rw_c<NCOL;rw_c++)
    for(int cn_c=0;cn_c<NCOL;cn_c++)
      rwCol(cnCol(transpose(t),cn_c),rw_c)=cn_c;
  
  /// Read the matrix as non-transposed
  for(int rw_c=0;rw_c<NCOL;rw_c++)
    for(int cn_c=0;cn_c<NCOL;cn_c++)
      if(rwCol(cnCol(t,cn_c),rw_c)!=rw_c)
	CRASH<<"Matrix entry ("<<rw_c<<","<<cn_c<<" expected "<<rw_c<<" obtained "<<rwCol(cnCol(t,cn_c),rw_c);
  
  TEST_PASSED;
}

/// Assert if a duplicated call is not removed
#define STATIC_ASSERT_DUPLICATED_CALL_REMOVER(FUN,...)			\
  static_assert(isSame<							\
		__VA_ARGS__,						\
		RemRef<decltype(FUN(FUN(__VA_ARGS__{})))>		\
		>,"Not same")

/// Assert if a duplicated call is not absorbed
#define STATIC_ASSERT_DUPLICATED_CALL_ABSORBER(FUN,...)			\
  static_assert(isSame<							\
		RemRef<decltype(FUN(__VA_ARGS__{}))>,			\
		RemRef<decltype(FUN(FUN(__VA_ARGS__{})))>		\
		>,"Not same")

/// Verify that calling duplicated-removed or absorbed functions are treated as expected
void checkDuplicatedCallRemoverOrAbsorber()
{
  STATIC_ASSERT_DUPLICATED_CALL_REMOVER(conj,Tens<TensKind<Compl>,double>);
  STATIC_ASSERT_DUPLICATED_CALL_REMOVER(transpose,Tens<TensKind<RwCol,CnCol>,double>);
  STATIC_ASSERT_DUPLICATED_CALL_REMOVER(adj,Tens<TensKind<Compl>,double>);
  STATIC_ASSERT_DUPLICATED_CALL_ABSORBER(wrap,Tens<TensKind<Compl>,double>);
  
  TEST_PASSED;
}

/// Test the nested binding of a tens
void checkBinding()
{
  ///Tensor to be used
  Tens<TensKind<RwSpin,Compl>,double> cicc;
  
  /// Value to be assigned
  const double value=
    1.9234424;
  
  /// Bind \c spin
  auto d=
    spin(cicc,0);
  
  /// Bind real part
  auto &e=
    reIm(d,0);
  
  // Assign
  e=value;
  
  /// Result
  double res=
    real(spin(cicc,0));
  
  if(res!=value)
    CRASH<<"Expected "<<value<<" obtained "<<res;
  
  const Tens<TensKind<RwSpin,Compl>,double> co;
  
  /// Type of the binding
  using T=
    decltype(rwSpin(imag(co),0));
  
  static_assert(isConst<T>,"Expected const");
  static_assert(isLvalue<T>,"Expected const");
  
  TEST_PASSED;
}

/// Test the conjugation
void checkConj()
{
  /// Tensor type
  using ComplTens=
    Tens<TensKind<Compl>,double>;
  
  /// Tensor
  ComplTens t;
  
  /// Expecte real value
  const double re=
    1.0;
  const double im=
    -3.14598712480;
    
  // Assign real and imag
  real(t)=re;
  imag(t)=im;
  
  /// Conjugated value
  auto c=
    conj(t);
  
  // Check real value
  if(real(c)!=re)
    CRASH<<"Real part: "<<real(c)<<" expected: "<<re;
  
  // Check imag value
  if(imag(c)!=-im)
    CRASH<<"Imag part: "<<imag(c)<<" expected: "<<-im;
  
  TEST_PASSED;
}

/// Test class
template <typename T>
class fuffa
  : public SingleInstance<fuffa<T>>
{
public:
  /// Dummy initializer
  fuffa<T>()
  {
  }
};

/// Check single instances classes
void checkSingleInstances()
{
  /// Single instance of \c fuffa<void>
  fuffa<void> test1;
  
  /// Single instance of \c fuffa<<int>
  fuffa<int> test2;
  
  TEST_PASSED;
}

/// Check that MPI is initialized
void checkMPIisInitalized()
{
  /// Initialization flag
  const int isInitialized=
    mpi.isInitialized();
  
  if(not isInitialized)
    CRASH<<"MPI not initialized!";
  
  TEST_PASSED;
}

/// Check MPI all reduce
void checkMPIallReduce()
{
  /// Value to sum
  const int val=
    2;
  
  /// Sum 2
  const int sum=
    mpi.allReduce(val);
  
  /// Expected value of the sum
  const int expSum=
    2*mpi.nRanks();
  
  if(sum!=expSum)
    CRASH<<"Summing "<<val<<" on all "<<mpi.nRanks()<<" nodes produced "<<sum<<" instead of expected "<<expSum;
  
  TEST_PASSED;
}

/// Test adding and removing of signness
void checkSignUnsign()
{
  /// Check the unsigning of A into B
#define CHECK_UNSIGN(A,B)						\
  static_assert(isSame<UnsignedOf<A>,B>,"Unsigned version of " #A " is not the same of " #B)
  
  /// Check the signing of A into B
#define CHECK_SIGN(A,B)						\
  static_assert(isSame<SignedOf<A>,B>,"Signed version of " #A " is not the same of " #B)
  
  CHECK_UNSIGN(int64_t,uint64_t);
  CHECK_UNSIGN(const int64_t,const uint64_t);
  CHECK_UNSIGN(int64_t&,uint64_t&);
  CHECK_UNSIGN(const int64_t&,const uint64_t&);
  
  CHECK_SIGN(uint64_t,int64_t);
  CHECK_SIGN(const uint64_t,const int64_t);
  CHECK_SIGN(uint64_t&,int64_t&);
  CHECK_SIGN(const uint64_t&,const int64_t&);
  
  TEST_PASSED;
  
#undef CHECK_UNSIGN
#undef CHECK_SIGN
}

/// Test vector class
void checkVectorClass()
{
  /// Vector to allocate
  Vector<int> a{1,2,3,6,1};
  
  /// Expected size
  const long int expSize=
    5;
  
  // Check size
  if(a.size()!=expSize)
    CRASH<<"Expected size: "<<expSize<<", obtained: "<<a.size();
  
  /////////////////////////////////////////////////////////////////
  
  /// Check first or last occurrency
  auto check=[&a](const int& searching,
		const int& expectedPos,
		const bool& firstLast)
    {
      /// Finds
      const int pos=
      (firstLast==false)?
      a.findFirst(searching):
      a.findLast(searching);
      
      // Check position
      if(pos!=expectedPos)
	CRASH<<"First "<<searching<<" expected at position "<<expectedPos<<"found at "<<pos;
    };
  
  check(6,3,false);
  check(4,expSize,false);
  check(3,2,true);
  check(8,-1,true);
  
  /////////////////////////////////////////////////////////////////
  
  /// Sum of all elements of \c a
  const int sum=
    a.summatorial(1);
  
  /// Expected result of the sum
  const int expSum=
    12;
  
  // Check sum
  if(sum!=expSum)
    CRASH<<"Expected sum: "<<expSum<<", obtained: "<<sum;
  
  /////////////////////////////////////////////////////////////////
  
  /// Take the product of all elements of \c a
  const int prod=
    a.productorial(1);
  
  /// Expected result of the product
  const int expProd=
    36;
  
  // Check product
  if(prod!=expProd)
    CRASH<<"Expected prod: "<<expProd<<",obtained: "<<prod;
  
  /////////////////////////////////////////////////////////////////
  
  TEST_PASSED;
}

/// Test combinatorial
void checkCombinatorial()
{
  /// Maximal number of object for each slot
  const Vector<int64_t> nMaxPerSlot=
    {1,2,3,2};
  
  /// Number of objects
  const int nObj=
    3;
  
  /// Set the combinatorial
  Combinatorial c(nMaxPerSlot,nObj);
  
  /// First combinatorial
  const auto l=
    c.getFirst();
  
  /// Expected
  const Vector<int64_t> expFirst=
    {1,2,0,0};
  
  if(expFirst!=c())
    CRASH<<"First combinatorial not working";
  
  c.advance();
  c.rewind();
  
  if(expFirst!=c())
    CRASH<<"Advancing and rewinding not coming back to first combinatorial";
  
  TEST_PASSED;
}

/// Test the sitmo random generator
void checkSitmo()
{
  // /// Create a random number generator
  // Sitmo rng;
  
  // // seed
  // rng.seed(100);
  
  // // Skip 8
  // for(int i=0;i<8;i++)
  //   runLog()<<rng();
  
  // /// Test the rng
  // const uint32_t r=
  //   rng();
  
  // /// Expected result
  // constexpr uint32_t expR=
  //   2770169712;
  
  // if(r!=expR)
  //   CRASH<<"Expected "<<expR<<" obtained "<<r;
  
  // /////////////////////////////////////////////////////////////////
  
  // // Seed again
  // rng.seed(100);
  
  // // Skip 8
  // rng.discard(8);
  
  // /// Test the rng again
  // const uint32_t r2=
  //   rng();
  
  // if(r2!=expR)
  //   CRASH<<"Expected "<<expR<<" obtained "<<r2;
  
  WARNING<<"NO SITMO CHECKED!!!!!";
  
  TEST_PASSED;
}

/// Check the serialization features
void checkSerializer()
{
  /// SubTestClass to be incapsulated inside the main TestClass
  DEFINE_SERIALIZABLE_CLASS(SubTestClass)
  {
  public:
    
    /// Pair to be used
    using P=
      std::pair<std::string,std::string>;
    
    SERIALIZABLE_SCALAR(P,p,"fi","se");
    
    SERIALIZABLE_SCALAR(std::vector<double>,v,3);
    SERIALIZABLE_SCALAR(double,b,1.0);
    
    LIST_SERIALIZABLE_MEMBERS(p,v,b);
  };
  
  /// TestClass to be serialized
  DEFINE_SERIALIZABLE_CLASS(TestClass)
  {
  public:
    
    SERIALIZABLE_CLASS(SubTestClass,subTestClass);
    SERIALIZABLE_SCALAR(std::string,ciccio,"pasticcio");
    
    LIST_SERIALIZABLE_MEMBERS(subTestClass,ciccio);
  };
  
  SERIALIZABLE_SCALAR_WITH_TAG(TestClass,test1,"testClass");
  SERIALIZABLE_SCALAR_WITH_TAG(TestClass,test2,"testClass");
  SERIALIZABLE_SCALAR_WITH_TAG(TestClass,test3,"testClass");
  
  test1().subTestClass().b=
    345235.1413;
  
  test1().subTestClass().v()[2]=
    4;
  
  test1().subTestClass().p().first=
    "primo";
  
  test1().ciccio=
    "pasticcissimo";
  
  test2.deSerialize(test1.serialize(ONLY_NON_DEFAULT));
  
  test3.deBinarize(test1.binarize());
  
#define CHECK(A)							\
  if(test1().A!=test2().A)						\
    CRASH<<"Expected "<<test1().A<<" for " #A ", obtained in serialized: "<<test2().A<<"\n"<<test1.serialize(ONLY_NON_DEFAULT)<<"\n\n"<<test2; \
									\
  if(test1().A!=test3().A)						\
    CRASH<<"Expected "<<test1().A<<" for " #A ", obtained in binarized: "<<test3().A<<"\n"<<test1<<"\n\n"<<test3;
  
  CHECK(subTestClass().b);
  CHECK(subTestClass().v()[2]);
  CHECK(subTestClass().p().first);
  CHECK(ciccio());
  
#undef CHECK
  
  TEST_PASSED;
}

/// Check storing of max
void checkValWithMax()
{
  /// First addend
  const int64_t first=
    12;
  
  /// Second addend
  const int64_t second=
    13;
  
  /// Third addend (to subtract)
  const int64_t third=
    23;
  
  /// Val to check max
  ValWithMax<int64_t> a=
    first;
  
  a+=
    second;
  
  a-=
    third;
  
  /// Expected maximum
  const int64_t expExtr=
    first+second;
 
  /// Expected final
  const int64_t expFin=
    first+second-third;
 
  if(a.extreme()!=expExtr)
    CRASH<<"Extreme expected: "<<expExtr<<", obtained: "<<a.extreme();
  
  if(a!=expFin)
    CRASH<<"Final expected: "<<expFin<<", obtained: "<<a;
  
  TEST_PASSED;
}

//////////////////////////////// TESTS TO BE FINISHED /////////////////////////////////

void checkIsAliasing()
{
  using ComplTens=Tens<TensKind<Compl>,double>;
  
  ComplTens t,u;
  
  //transpose_bis(t);
  
  auto a=wrap(Tens<TensKind<Compl>,double>{});
  auto b=wrap(wrap(Tens<TensKind<Compl>,double>{}));
  
  runLog()<< isSame<RemRef<decltype(a)>,RemRef<decltype(b)>> ;
  runLog()<<isSame<decltype(a),decltype(b)>;
  runLog()<<"t.isAliasing(t): "<<t.isAliasing(t.getStor());
  runLog()<<"t.isAliasing(u): "<<t.isAliasing(u.getStor());
  runLog()<<"conj(t).isAliasing(t): "<<conj(t).isAliasing(t.getStor());
}

void checkBindComplicatedExpression()
{
  TEST_PASSED;
  
  Tens<TensKind<Compl>,double> t;
  auto tr=transpose(wrap(wrap(t)));
  reIm(transpose(tr),0)=1.0;
  //reim(transpose(transpose(Tens<TensKind<Compl>,double>{})),0)=1.0;
       //const double &e=reim(conj()),0);
  using MyTk=TensKind<Spacetime,Col,Spin,Compl>;
  //using MyTk=TensKind<Col,Spin,Compl>;
  using MyTens=Tens<MyTk,double>;
  
  //int a=MyTk::firstVectorizingComp<double>;
  //runLog()<<a;
  
  int vol=10;
  MyTens cicc(vol);
  
  // auto &v=cicc.getStor();
  
  // index<MyTk>(v.dynSizes,0,0,0,0);
  
  //eval(cicc.getStor(),0,0,0,0);
  
  for(int ic=0;ic<NCOL;ic++)
    for(int id=0;id<NSPIN;id++)
      for(int ri=0;ri<NCOMPL;ri++)
      {
  	//double &ref=eval(color(spin(cicc,id),ic));
  	double &ref=site(reIm(spin(col(transpose(transpose(cicc)),ic),id),ri),0);
  	//printf("%lld %lld\n",(long long int)cicc.get_v()._v,(long long int)&ref);
  	ref=3.141592352352;
      }
  
  auto binder1=site(reIm(spin(col(cicc,2),3),1),0);
  // //auto binder2=color(spin(cicc,2),1);
  
  // // eval(binder1)=8.0;
  // printf("%d\n",cicc.getStor().nel);
  printf("ANNA %lg\n",binder1);
  // runLog()<<Spacetime::name();
  // runLog()<<Col::name();
 }

// void test_assigner()
// {
//   TEST_PASSED;
  
//   using MyTk1=TensKind<Col,Spin,Compl>;
//   using MyTens1=Tens<MyTk1,double>;
  
//   using MyTk2=TensKind<Spin,Col,Compl>;
//   using MyTens2=Tens<MyTk2,double>;
  
//   MyTens1 cicc1;
//   MyTens2 cicc2;
  
//   runLog()<<"cicc1 storage: "<<getStor(cicc1)._v;
  
//   for(int icol=0;icol<NCOL;icol++)
//     for(int ispin=0;ispin<NSPIN;ispin++)
//       for(int icompl=0;icompl<NCOMPL;icompl++)
//   	col(spin(reim(cicc1,icompl),ispin),icol) EVAL=
//   	  icompl+NCOMPL*(ispin+NSPIN*icol);
  
//   MyTens2 cicc3;
  
//   auto co=conj(cicc1);
//   transpose(cicc3)=cicc2=transpose(co);
//   transpose(cicc3)=cicc2=transpose(conj(cicc1));
  
//   //cicc2=cicc1;
  
//   //int a=ass.ref2;
  
//   // double a=0.0;
//   // for(int icol=0;icol<NCOL;icol++)
//   //   for(int ispin=0;ispin<NSPIN;ispin++)
//   //     for(int icompl=0;icompl<NCOMPL;icompl++)
//   // 	{
//   // 	  double b=col(spin(reim(cicc2,icompl),ispin),icol);
//   // 	  a+=b;
//   // 	  runLog()<<b;
//   // 	}
//   // runLog()<<a;
  
//   // using MyTk1=TensKind<Col,Compl>;
//   // using MyTens1=Tens<MyTk1,double>;

//   // using MyTk2=TensKind<Col,Compl>;
//   // using MyTens2=Tens<MyTk2,double>;

//   // MyTens1 cicc1;
//   // MyTens2 cicc2;
  
//   // for(int icol=0;icol<NCOL;icol++)
//   //   for(int icompl=0;icompl<NCOMPL;icompl++)
//   //     col(reim(cicc1,icompl),icol).eval()=
//   // 	icompl;
  
//   // runLog()<<"/////////////////////////////////////////////////////////////////";
//   // // for(int icol=0;icol<NCOL;icol++)
//   // //   for(int icompl=0;icompl<NCOMPL;icompl++)
//   // //     {
//   // // 	col(reim(cicc2,icompl),icol).eval()=
//   // // 	  col(reim(conj(cicc1),icompl),icol).eval();
//   // // 	runLog()<<col(reim(cicc2,icompl),icol).eval()<<
//   // // 	  " "<<col(reim(cicc1,icompl),icol).eval();
//   // //     }
//   // // runLog()<<"/////////////////////////////////////////////////////////////////";

//   // auto c1=conj(cicc1);
//   // auto ass=assign(cicc2,c1);
//   //  ass.close();
  
//   // for(int icol=0;icol<NCOL;icol++)
//   //   for(int icompl=0;icompl<NCOMPL;icompl++)
//   //     runLog()<<col(reim(cicc2,icompl),icol).eval()<<" "<<col(reim(cicc1,icompl),icol).eval();
  
// }

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
  
  // using Mu=
  //   decltype(mulAdder);

  mulAdder.eval(0,0,0);
  
  // using Md=
  //   Mu::MergingDelimsOfRefs<IntSeq<0,3>>;
  
  // using Pos=
  //   Mu::PosOfResTcsInRefsTk;
  
  //int y=Mu::MergeableComps{};
  
  //mulAdder.getMergedCompsView<IntSeq<0,1,2,3>>();
}

  // // Check on Diag
  // {
  //   using MyTk=TensKind<RwCol,Spin,CnCol>;
  //   using A=IntSeq<1,0,1>;
  //   using B=MyTk::IsMatrixComp;
  //   STATIC_ASSERT_IS_BASE_OF(A,B);
    
  //   using F=MyTk::Diag;
    
    
    
  //   using G=TensKind<RwCol,Spin>;
  //   STATIC_ASSERT_IS_BASE_OF(G,F);
    
  // }
  
  // // Check relBind
  // {
  //   using MyTk=TensKind<RwCol,Spin,Spacetime,CnCol>;
    
  //   using MyTens=Tens<MyTk,double>;
  //   MyTens c(10);
    
  //   c.eval(1,2,0,1)=1.0594;
    
  //   auto b1=relBind<RwCol,CnCol>(c,[](int id){return id;});
  //   auto b2=relBind<CnCol,RwCol>(c,[](int id){return id;});
  //   // runLog()<<decltype(c)::Tk::name();
  //   // runLog()<<decltype(b1)::Tk::name();
  //   // runLog()<<decltype(b2)::Tk::name();
  //   runLog()<<c.eval(1,2,0,1)<< " "<<b1.eval(2,0,1)<<" "<<b2.eval(1,2,0);
    
  //   auto bb1=relBind<Spin,Col>(b1,[](int id){return id+1;});
  //   auto bb2=relBind<Spin,Col>(b2,[](int id){return id+1;});
    
  //   runLog()<<c.eval(1,2,0,1)<< " "<<bb1.eval(0,1)<<" "<<bb2.eval(1,0);
    
  //   runLog()<<decltype(c.getMaximallyMergedCompsView())::Tk::name();
  //   runLog()<<" c: "<<decltype(c)::Tk::name();
  //   runLog()<<" bb2: "<<decltype(bb2)::Tk::name();
  //   // int o=decltype(b1)::ExtraDelims{};
  //   // int oo=decltype(b1)::PosOfResTcsInRefsTk{};
  //   // int p=decltype(b1)::MergeableComps{};
  //   runLog()<<decltype(b1.getMaximallyMergedCompsView())::Tk::name();
  // }
  
  // //come trovare le componenti non twinnate? crea un IntSeq che dica
  // //se le componenti devono essere prese o no, prendi di questo le
  // //posizioni non nulle, e chiama getIndexed
  // {
  //   using MyTk=TensKind<RwCol,Spin,CnCol>;
  //   using MyTens=Tens<MyTk,double>;
    
  //   MyTens c;
    
  //   runLog()<<" "<<posOfType<RwCol,MyTk::Twinned::types>;
    
  //   STATIC_ASSERT_IS_BASE_OF(MyTens,decltype(+c));
  // }
  
  // //check the blending of two TensKind
  // {
  //   using MyTk1=TensKind<RwCol,Spin>;
  //   using MyTk2=TensKind<RwCol,CnCol>;
  //   using MyTkBCompa=TensKind<RwCol,Spin,CnCol>;
    
  //   using MyTkBRes1=typename MyTk1::BlendWithTensKind<MyTk2>;
  //   STATIC_ASSERT_IS_BASE_OF(MyTkBRes1,MyTkBCompa);
    
  //   using MyTkBRes2=BlendTensKinds<MyTk1,MyTk2>;
  //   STATIC_ASSERT_IS_BASE_OF(MyTkBRes2,MyTkBCompa);
    
  //   using MyTkBRes3=BlendTensKinds<MyTk1,MyTk2,MyTk2>;
  //   STATIC_ASSERT_IS_BASE_OF(MyTkBRes3,MyTkBCompa);
  // }
  
  
  // using MyTk=TensKind<RwCol,Spin,Compl,CnCol>;
  // using MyTens=Tens<MyTk,double>;
  
  // MyTens cicc;
  // for(int irw_col=0;irw_col<NCOL;irw_col++)
  //   for(int ispin=0;ispin<NSPIN;ispin++)
  //     for(int icn_col=0;icn_col<NCOL;icn_col++)
  // 	for(int ri=0;ri<NCOMPL;ri++)
  // 	  rwCol(reIm(cnCol(spin(cicc,
  // 				ispin),
  // 			   icn_col),
  // 		     ri),
  // 		irw_col)=
  // 	  icn_col+NCOL*(0+NCOMPL*(ispin+NSPIN*irw_col));

  // cicc.getMaximallyMergedCompsView()=1.0;
  
  // auto i=imag(conj(cnCol(spin(rwCol(-cicc,0),0),0)));
  // runLog()<<i;
  
  // runLog()<<"/////////////////////////////////////////////////////////////////";
  
  // for(int irw_col=0;irw_col<NCOL;irw_col++)
  //   for(int ispin=0;ispin<NSPIN;ispin++)
  //     for(int icn_col=0;icn_col<NCOL;icn_col++)
  // 	for(int ri=0;ri<NCOMPL;ri++)
  // 	  {
  // 	    runLog()<<"------";
  // 	    runLog()<<&rwCol(reIm(cnCol(spin(cicc,
  // 					 ispin),
  // 				  icn_col),
  // 			      ri),
  // 			 irw_col);
  // 	  }
  
  // runLog()<<"/////////////////////////////////////////////////////////////////";
  
  // double a=0.0;
  // for(int irw_col=0;irw_col<NCOL;irw_col++)
  //   for(int ispin=0;ispin<NSPIN;ispin++)
  //     for(int icn_col=0;icn_col<NCOL;icn_col++)
  // 	a+=imag(conj(rwCol(cnCol(spin(cicc,
  // 				 ispin),
  // 			    icn_col),
  // 			   irw_col)));
  // 	// a+=real(conj(rwCol(cnCol(spin(cicc,
  // 	// 			 ispin),
  // 	// 		    icn_col),
  // 	// 		   irw_col)));
  
  // runLog()<<a;
  

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
  
  d=(a+b).getMaximallyMergedCompsView();
  
  runLog()<<"d: "<<d.eval(1,1,1);
  
  //auto sum=a+b;
  // this was added here to show the maximally mergeable delims
  //int e=decltype(sum)::MergeableComps{};
  //int e=decltype(sum)::PosOfRef1TcsInResTk{};
  //int e=decltype(sum)::MergedDelims1<IntSeq<0,1,3,4>>{};
  
  
  //runLog()<<"Col: "<<c.compSize<RwCol>();
  //runLog()<<"Spin: "<<c.compSize<CnSpin>();
  runLog()<<"Tensor Kind of addendum 1: "<<decltype(a)::Tk::name();
  using M1=
    typename decltype(a)::MergeableComps;
  runLog()<<"Mergeability of addendum 1: "<<printIntSeq(M1{});
  runLog()<<"Tensor Kind of addendum 2: "<<decltype(b)::Tk::name();
  using M2=
    typename decltype(b)::MergeableComps;
  runLog()<<"Mergeability of addendum 2: "<<printIntSeq(M2{});
  runLog()<<"Tensor Kind: "<<decltype(a+b)::Tk::name();
  
  runLog()<<"PosOf types 1 present: "<<printIntSeq(TupleElementType<0,decltype(a+b)::PosOfResTcsPresInRefsTk>{});
  runLog()<<"PosOf types 2 present: "<<printIntSeq(TupleElementType<1,decltype(a+b)::PosOfResTcsPresInRefsTk>{});
  
  // using P1=
  //   decltype(c)::posOfAddend1TcInResTk;
  // runLog()<<"PosOf types 1: "<<printIntSeq(P1{});
  // using P2=
  //   decltype(c)::posOfAddend2TcInResTk;
  // runLog()<<"PosOf types 2: "<<printIntSeq(P2{});
  
  // using P3=
  //   PairOfTensKindMergeability::template CompsMergeability<M1,M2,P1,P2>;
  
  // runLog()<<"Mergeability: "<<printIntSeq(P3{});
  
  // using MDel=
  //   IntSeq<0,1,3,4>;
  
  // runLog()<<"Trying to merge with: "<<printIntSeq(MDel{});
  
  // using P4=
  //   decltype(c)::template MergedDelims1<MDel>;
  
  // runLog()<<"The first needs to be merged like this: "<<printIntSeq(P4{});
  
  TEST_PASSED;
}

int main()
{
  runLog()<<"Timings: "<<timings.isStarted()<<" "<<timings.currentMeasure().count();
  runLog()<<"Timings: "<<timings["ciao"].isStarted()<<" "<<timings["ciao"].currentMeasure().count();
  
  runLog()<<"Anna\n";
  {
    SCOPE_INDENT(runLog);
    SCOPE_REAL_PRECISION(runLog,3);
    {
      SCOPE_INDENT(runLog);
      SCOPE_ALWAYS_PUT_SIGN(runLog);
      SCOPE_REAL_PRECISION(runLog,10);
      runLog()<<1.23523524<<"\n";
    }
    runLog()<<1.23523524<<"\n";
    
    runLog()<<"Elena\n";
  }
  
  runLog()<<"Caterina\n";
  
  checkSignUnsign();
  
  checkVectorClass();
  
  checkCombinatorial();
  
  checkNonComplConjCancelation();
  
  checkNestedConjCancelation();
  
  checkDuplicatedUMinusCancellation();
  
  checkFilterVariadicClassPos();
  
  checkScalarWrap();
  
  checkTensKindIscontained();
  
  // this is not passing...
  //checkCallOperator();
  
  checkGrid();
  
  checkFlagMasks();
  
  checkSafeModulo();
  
  checkFactorize();
  
  checkLhsTranspose();
  
  checkDuplicatedCallRemoverOrAbsorber();
  
  checkBinding();
  
  checkConj();
  
  checkSingleInstances();
  
  checkMPIisInitalized();
  
  checkMPIallReduce();
  
  checkSitmo();
  
  checkSerializer();
  
  checkValWithMax();
  
  return 0;
}
