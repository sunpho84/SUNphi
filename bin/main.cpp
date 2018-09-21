//#define DEBUG_TENS
//#define DEBUG_BINDER
//#define DEBUG_INDEXER

#include <SUNphi.hpp>

using namespace std;
using namespace SUNphi;


//
int main()
{
  // Check ability to filter
  {
    using A=FilterVariadicClassPos<IsNotNull, IntSeq<0,1,0,10>>;
    using B=IntSeq<1,3>;
    STATIC_ASSERT_IS_BASE_OF(A,B);
  }

  {
    using MyTk=TensKind<RwCol,Spin,CnCol>;
    using A=IntSeq<1,0,1>;
    using B=MyTk::IsMatrixComp;
    STATIC_ASSERT_IS_BASE_OF(A,B);
    
    using F=MyTk::Diag;
    
    
    
    using G=TensKind<RwCol,Spin>;
    STATIC_ASSERT_IS_BASE_OF(G,F);
    
  }
  
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
