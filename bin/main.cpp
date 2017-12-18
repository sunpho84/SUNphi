#include <SUNphi.hpp>

#include <array>
#include <iostream>

using namespace std;
using namespace SUNphi;

constexpr int NCOL=3;
constexpr int NSPIN=4;

struct ColorKind : public BaseTensCompKind
{
};

struct SpinKind : public BaseTensCompKind
{
};

struct Color : public TensComp<ColorKind,NCOL>
{
  static string name()
  {
    return "color";
  }
};

struct Spin : public TensComp<SpinKind,NSPIN>
{
  static string name()
  {
    return "spin";
  }
};

template <typename Tg,typename Tk>
auto bind(Tk&& ref,const int id)
{
  return Binder<Tg,Tk>(std::forward<Tk>(ref),id);
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

template <typename Tg,                                                // Type to get
	  typename InNested,                                          // Type referred from the nested bounder
	  typename InNestedTg>                                        // Type got by the nested bounder
auto bind(Binder<InNestedTg,InNested>&& ref,const Tg&,const int id)
{
  /// Tensor Kind of input nested binder
  using InNestedTk=typename RemoveReference<InNested>::Tk;
  /// Types of the Tensor Kind of nested bounder
  using NestedTypes=typename InNestedTk::Types;
  /// Position inside the nested reference of the type got by the nested bounder
  constexpr int InNestedNestedTgPos=posOfType<InNestedTg,NestedTypes>;
  /// Position inside the nested reference of the type to get
  static constexpr int NestedTgPos=posOfType<Tg,NestedTypes>;
  /// Keep note of whether we need to swap
  constexpr bool swap=(NestedTgPos>InNestedNestedTgPos);
  /// Type got by the output nested binder
  using OutNestedTg=Conditional<swap,Tg,InNestedTg>;
  /// Type got by the output binder
  using OutTg=Conditional<swap,InNestedTg,Tg>;
  /// Nested component
  const int nestedId=ref.id;
  /// Out external component
  const int outNestedId=(swap?id:nestedId);
  /// Out component
  const int outId=(swap?nestedId:id);
  /// Output Nested binder
  auto outNestedBinder=bind<OutNestedTg>(std::forward<InNested>(ref.ref),outNestedId);
  /// Type of the output nested binder
  using OutNestedBinder=decltype(outNestedBinder);
  
  // cout<<"Constructing a nested binder for type "<<Tg::name()<<", internal binder gets: "<<InNestedTg::name()<<", swap: "<<swap<<endl;
  // cout<<"OutTg: "<<OutTg::name()<<" "<<endl;
  return Binder<OutTg,OutNestedBinder>(std::move(outNestedBinder),outId);
}

template <class T>
auto color(T&& ref,const int id)
{
  return bind<Color>(std::forward<T>(ref),id);
}

template <class T>
auto spin(T&& ref,const int id)
{
  return bind<Spin>(std::forward<T>(ref),id);
}

// template <class...Tp>
// constexpr int countUniqueTypes=Sum<(countTypeIsSame<Tp,Tp...> ==1)...>;

using MyTk=TensKind<Color,Spin>;

int main()
{
  Tens<MyTk,double> cicc;
  
  for(int ic=0;ic<3;ic++)
    for(int id=0;id<4;id++)
      {
	//double &ref=eval(color(spin(cicc,id),ic));
	double &ref=eval(spin(color(cicc,ic),id));
	printf("%lld %lld\n",(long long int)cicc.get_v()._v,(long long int)&ref);
	ref=3.141592352352;
      }
  
  auto binder1=spin(color(cicc,1),3);
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
