//#define DEBUG_TENS
//#define DEBUG_BINDER
//#define DEBUG_INDEXER

#include <SUNphi.hpp>

using namespace std;
using namespace SUNphi;

template <int P,
	  typename T,
	  template <T A> typename F,
	  int...Is>
class _TruePosOfVariadicClass;

template <int P,
	  typename T,
	  template <T A> class F>
class _TruePosOfVariadicClass<P,T,F>
{
public:
  using Out=IntSeq<>;
};

template <int I>
class Fun
{
public:
  static constexpr bool res=(I!=0);
};

template <int P,
	  typename T,
	  template <int A> class F,
	  int I,
	  int...Oth>
class _TruePosOfVariadicClass<P,T,F,I,Oth...>
{
public:
  using Out=IntSeqCat<Conditional< F<I>::res,
				  IntSeq<P>,
				  IntSeq<>>,
                      typename _TruePosOfVariadicClass<P+1,T,F,Oth...>::Out>;
};

template <typename T,
	  template <T A> class F,
	  T...I>
using TruePosOfVariadicClass=typename _TruePosOfVariadicClass<0,T,F,I...>::Out;

int main()
{
  using A=TruePosOfVariadicClass<int,Fun, 0,1,0,10>;
  
  A aa;
  //int aaa=aa;
  
  
  // Check that conj of a non-complex type object is the same of original type
  {
    using MyTk=TensKind<RwCol,Spin,CnCol>;
    using MyTens=Tens<MyTk,double>;
    
    MyTens c;
    
    STATIC_ASSERT_IS_BASE_OF(MyTens,decltype(conj(c)));
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
  
  // Check that repeated - is absorbed
  {
    using MyTk=TensKind<RwCol,Spin,CnCol>;
    using MyTens=Tens<MyTk,double>;
    
    MyTens c;
    
    STATIC_ASSERT_IS_BASE_OF(MyTens,decltype(-(-c)));
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
