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

template <typename Tk,class Tg>
auto bind(Tk& ref,const Tg&,const int id)
{
  return Binder<Tk,Tg>(ref,id);
}

template <typename Nested,typename NestedTg,class Tg>
auto bind(Binder<Nested,NestedTg>& ref,const Tg&,const int id)
{
  using NestedTk=typename Nested::TK;
  using Tk=Binder<Nested,NestedTg>;
  using NestedTypes=typename NestedTk::Types;
  constexpr int nestedNestedTgPos=posOfType<NestedTg,NestedTypes>;
  constexpr int nestedTgPos=posOfType<Tg,NestedTypes>;
  const int nestedId=ref.id;
  
  return (nestedTgPos>nestedNestedTgPos)?
    Binder<Tk,Tg>(ref,id):
    bind(bind(ref.ref,Tg{},id),NestedTg{},nestedId);
}



template <typename T,class Tg>
auto bind(T&& ref,const int id)
{
  return Binder<T,Tg>(std::forward<T>(ref),id);
}

namespace SUNphi
{
  namespace Impl
{
  template <typename Nested,                                        // Type referred from the nested bounder
	    typename NestedTg,                                      // Type got by the nested bounder
	    class Tg,                                               // Type to get
	    class B=Binder<Nested,NestedTg>,                        // Type to be bound
	    class NestedTK=typename Nested::TK,                     // Tensor Kind of nested bounder
	    class NestedTypes=typename NestedTK::Types,             // Types of the Tensor Kind of nested bounder
	    int NestedNestedTgPos=posOfType<NestedTg,NestedTypes>,  // Position inside the nested reference of the type got by the nested bounder
	    int NestedTgPos=posOfType<Tg,NestedTypes>,              // Position inside the nested reference of the type to get
	    int Swap=(NestedTgPos>NestedNestedTgPos)>
  struct NestedBinder
  {
    typedef SUNphi::Conditional<Swap,Tg,NestedTg> intTg;
    typedef SUNphi::Conditional<Swap,NestedTg,Tg> extTg;
    typedef Binder<Nested,intTg> intBinder;
    typedef Binder<intBinder,extTg> extBinder;
    
    extBinder binder;
    
    NestedBinder(Binder<Nested,NestedTg>&& ref,const int id) : binder(intBinder(ref.ref,(Swap?id:ref.id)),(Swap?ref.id:id)) {}
  };
  }
}

template <typename Nested,
	  typename NestedTg,
	  class Tg>
auto bind(Binder<Nested,NestedTg>&& ref,const Tg&,const int id)
{
  return Impl::NestedBinder<Nested,NestedTg,Tg>(std::forward<Binder<Nested,NestedTg>>(ref),id).binder;
}

template <class T>
auto color(T&& ref,const int id)
{
  return bind(std::forward<T>(ref),Color{},id);
}

template <class T>
auto spin(T&& ref,const int id)
{
  return bind(std::forward<T>(ref),Spin{},id);
}

// template <class...Tp>
// constexpr int countUniqueTypes=Sum<(countTypeIsSame<Tp,Tp...> ==1)...>;

using MyTK=TensKind<Color,Spin>;

class MyTens
{
  using TS=TensStor<MyTK,double>;

public:
  
  typedef MyTK TK;
  TS v;
  
  static constexpr bool isStoring=true;
  
  template <class...Comps>
  friend double& eval(MyTens& t,const Comps&...comps)
  {
    static_assert(sizeof...(comps)==TK::nTypes,"The number of arguments must be the same of the number");
    
    return eval(t.v,std::forward<const Comps>(comps)...);
  }
};

int main()
{
  MyTens cicc;
  index<MyTK>(3.0,2);
  
  auto binder1=spin(color(cicc,1),2);
  auto binder2=color(spin(cicc,2),1);
  
  eval(binder1)=8.0;
  printf("%lg\n",eval(binder2));
  
// #ifdef WITH
//   auto binder1=spin(color(cicc,1),2);
//   eval(binder1)=8.0;
//   printf("%lg\n",eval(binder1));
// #else
//   cicc.v._v[6]=8.0;
//   printf("%lg\n",cicc.v._v[6]);
// #endif
  
// #ifdef WITH
//   printf("%d\n",Indexer<MyTK>::index(1,2));
// #else
//   printf("%d\n",6);
// #endif
//auto binder3=color(spin(cicc,2),1);
  
  //auto &ref=cicc;
  
  return 0;
}
