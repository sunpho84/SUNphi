#include <SUNphi.hpp>

#include <array>
#include <iostream>

using namespace std;
using namespace SUNphi;


using MyTk=TensKind<Spacetime,Col,Spin,Compl>;
using MyTens=Tens<MyTk,double>;

int main()
{
  
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
  	double &ref=eval(reim(spin(site(col(cicc,ic),0),id),ri));
  	//printf("%lld %lld\n",(long long int)cicc.get_v()._v,(long long int)&ref);
  	ref=3.141592352352;
      }
  
  auto binder1=site(reim(spin(col(cicc,2),3),1),0);
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
