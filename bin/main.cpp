#include <SUNphi.hpp>

#include <array>
#include <iostream>

using namespace std;
using namespace SUNphi;


//using MyTk=TensKind<Spacetime,Col,Spin,Compl>;
using MyTk=TensKind<Col,Spin,Compl>;
using MyTens=Tens<MyTk,double>;

void test_transpose()
{
  using Su3Tens=Tens<TensKind<RwCol,CnCol>,double>;
  
  Su3Tens t;
  
  for(int rw_c=0;rw_c<NCOL;rw_c++)
    for(int cn_c=0;cn_c<NCOL;cn_c++)
      eval(rwCol(cnCol(transpose(t),cn_c),rw_c))=cn_c;
  
  for(int rw_c=0;rw_c<NCOL;rw_c++)
    for(int cn_c=0;cn_c<NCOL;cn_c++)
      cout<<cn_c<<" "<<eval(rwCol(cnCol(t,cn_c),rw_c))<<endl;
}

int main()
{
  int a=MyTk::firstVectorizingComp<double>;
  cout<<a<<endl;
  test_transpose();
  
  //int a=MyTk::firstVectorizingComp<double>;
  //cout<<a<<endl;
  
 
  MyTens cicc;
  
  // auto &v=cicc.getStor();
  
  // index<MyTk>(v.dynSizes,0,0,0,0);
  
  //eval(cicc.getStor(),0,0,0,0);
  
  for(int ic=0;ic<NCOL;ic++)
    for(int id=0;id<NSPIN;id++)
      for(int ri=0;ri<NCOMPL;ri++)
      {
  	//double &ref=eval(color(spin(cicc,id),ic));
  	double &ref=eval(reim(spin(col(transpose(transpose(cicc)),ic),id),ri));
  	//printf("%lld %lld\n",(long long int)cicc.get_v()._v,(long long int)&ref);
  	ref=3.141592352352;
      }
  
  auto binder1=reim(spin(col(cicc,2),3),1);
  // //auto binder2=color(spin(cicc,2),1);
  
  // // eval(binder1)=8.0;
  // printf("%d\n",cicc.getStor().nel);
  printf("ANNA %lg\n",eval(binder1));
  // cout<<Spacetime::name()<<endl;
  // cout<<Col::name()<<endl;

// #ifdef WITH
//   auto binder1=spin(color(cicc,1),2);
//   eval(binder1)=8.0;
//   printf("%lg\n",eval(binder1));
// #else
//   cicc.v._v[6]=8.0;
//   printf("%lg\n",cicc.v._v[6]);
// #endif
  
// #ifdef WITH
//   printf("%d\n",Indexer<MyTk>::index(1,2));
// #else
//   printf("%d\n",6);
// #endif
//auto binder3=color(spin(cicc,2),1);
  
  //auto &ref=cicc;
  
  return 0;
}
