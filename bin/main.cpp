#include <SUNphi.hpp>

#include <array>
#include <iostream>

using String=std::string;

namespace SUNphi
{
  DEFINE_TENS_COMP(reim,Compl,NCOMPL,2);
  DEFINE_TENS_COMP(col,Col,NCOL,3);
  DEFINE_TENS_COMP(spin,Spin,NSPIN,4);
  
  /// Index of the real part of a \c Compl
  constexpr int REAL_PART_ID=0;
  
  /// Returns a reference to the real part
  template <typename T>
  auto real(T&& ref)
  {
    return reim(ref,REAL_PART_ID);
  }
  
  /// Index of the imag part of a \c Compl
  constexpr int IMAG_PART_ID=1;
  
  /// Returns a reference to the imag part
  template <typename T>
  auto imag(T&& ref)
  {
    return reim(ref,IMAG_PART_ID);
  }
}

  // template <typename Nested,typename NestedTg,class Tg>
  // auto bind(Binder<Nested,NestedTg>& ref,const Tg&,const int id)
  // {
  //   using NestedTk=typename Nested::Tk;
  //   using Tk=Binder<Nested,NestedTg>;
  //   using NestedTypes=typename NestedTk::Types;
  //   constexpr int nestedNestedTgPos=posOfType<NestedTg,NestedTypes>;
  //   constexpr int nestedTgPos=posOfType<Tg,NestedTypes>;
  //   const int nestedId=ref.id;
  
  //   return (nestedTgPos>nestedNestedTgPos)?
  //     Binder<Tk,Tg>(ref,id):
  //     bind(bind(ref.ref,Tg{},id),NestedTg{},nestedId);
  // }
  
  
  
  // template <typename T,class Tg>
  // auto bind(T&& ref,const int id)
  // {
  //   return Binder<T,Tg>(std::forward<T>(ref),id);
  // }
  

using namespace std;
using namespace SUNphi;

// template <class...Tp>
// constexpr int countUniqueTypes=Sum<(countTypeIsSame<Tp,Tp...> ==1)...>;

using MyTk=TensKind<Col,Spin,Compl>;

int main()
{
  Tens<MyTk,double> cicc;
  
  for(int ic=0;ic<NCOL;ic++)
    for(int id=0;id<NSPIN;id++)
      for(int ri=0;ri<NCOMPL;ri++)
      {
	//double &ref=eval(color(spin(cicc,id),ic));
	double &ref=eval(reim(spin(col(cicc,ic),id),ri));
	//printf("%lld %lld\n",(long long int)cicc.get_v()._v,(long long int)&ref);
	ref=3.141592352352;
      }
  
  auto binder1=reim(spin(col(cicc,2),3),1);
  //auto binder2=color(spin(cicc,2),1);
  
  // eval(binder1)=8.0;
  printf("ANNA %lg\n",eval(binder1));
  
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
