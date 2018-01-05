#include <SUNphi.hpp>

#include <array>
#include <iostream>

using namespace std;
using namespace SUNphi;


// void test_first_vectorizer()
// {
//   int a=MyTk::firstVectorizingComp<double>;
//   cout<<a<<endl;
// }

// void test_transpose()
// {
//   using Su3Tens=Tens<TensKind<RwCol,CnCol>,double>;
  
//   Su3Tens t;
  
//   for(int rw_c=0;rw_c<NCOL;rw_c++)
//     for(int cn_c=0;cn_c<NCOL;cn_c++)
//       eval(rwCol(cnCol(transpose(t),cn_c),rw_c))=cn_c;
  
//   for(int rw_c=0;rw_c<NCOL;rw_c++)
//     for(int cn_c=0;cn_c<NCOL;cn_c++)
//       cout<<cn_c<<" "<<eval(rwCol(cnCol(t,cn_c),rw_c))<<endl;
// }

#define STATIC_ASSERT_DUPLICATED_CALL_REMOVER(FUN,...)			\
  static_assert(IsSame<							\
		__VA_ARGS__,						\
		RemoveReference<decltype(FUN(FUN(__VA_ARGS__{})))>	\
		>,"Not same")

#define STATIC_ASSERT_DUPLICATED_CALL_ABSORBER(FUN,...)			\
  static_assert(IsSame<							\
		RemoveReference<decltype(FUN(__VA_ARGS__{}))>,		\
		RemoveReference<decltype(FUN(FUN(__VA_ARGS__{})))>	\
		>,"Not same")

void test_duplicated_call_remover()
{
  STATIC_ASSERT_DUPLICATED_CALL_REMOVER(conj,Tens<TensKind<Compl>,double>);
  STATIC_ASSERT_DUPLICATED_CALL_REMOVER(transpose,Tens<TensKind<RwCol,CnCol>,double>);
  STATIC_ASSERT_DUPLICATED_CALL_REMOVER(adj,Tens<TensKind<Compl>,double>);
  STATIC_ASSERT_DUPLICATED_CALL_ABSORBER(wrap,Tens<TensKind<Compl>,double>);
}

void test_isAliasing()
{
  using ComplTens=Tens<TensKind<Compl>,double>;
  
  ComplTens t,u;
  
  //transpose_bis(t);
  
  auto a=wrap(Tens<TensKind<Compl>,double>{});
  auto b=wrap(wrap(Tens<TensKind<Compl>,double>{}));
  
  cout<< IsSame<RemoveReference<decltype(a)>,RemoveReference<decltype(b)>> <<endl;
  cout<<IsSame<decltype(a),decltype(b)><<endl;
  cout<<"t.isAliasing(t): "<<t.isAliasing(t.getStor())<<endl;
  cout<<"t.isAliasing(u): "<<t.isAliasing(u.getStor())<<endl;
  cout<<"conj(t).isAliasing(t): "<<conj(t).isAliasing(t.getStor())<<endl;
}

// void test_conj()
// {
//   using ComplTens=Tens<TensKind<Compl>,double>;
  
//   ComplTens t;
  
//   eval(real(t))=1.0;
//   eval(imag(t))=-3.14598712480;
  
//   auto c1=t;
//   auto c2=conj(t);

//   printf("im: %lg",eval(reim(c2,0)));
//   printf("im: %lg",eval(reim(c2,1)));
// }

// void test_bind_complicated_expression()
// {
//   using MyTk=TensKind<Spacetime,Col,Spin,Compl>;
//   //using MyTk=TensKind<Col,Spin,Compl>;
//   using MyTens=Tens<MyTk,double>;

//   //int a=MyTk::firstVectorizingComp<double>;
//   //cout<<a<<endl;
  
//   int vol=10;
//   MyTens cicc(vol);
  
//   // auto &v=cicc.getStor();
  
//   // index<MyTk>(v.dynSizes,0,0,0,0);
  
//   //eval(cicc.getStor(),0,0,0,0);
  
//   for(int ic=0;ic<NCOL;ic++)
//     for(int id=0;id<NSPIN;id++)
//       for(int ri=0;ri<NCOMPL;ri++)
//       {
//   	//double &ref=eval(color(spin(cicc,id),ic));
//   	double &ref=eval(site(reim(spin(col(transpose(transpose(cicc)),ic),id),ri),0));
//   	//printf("%lld %lld\n",(long long int)cicc.get_v()._v,(long long int)&ref);
//   	ref=3.141592352352;
//       }
  
//   auto binder1=site(reim(spin(col(cicc,2),3),1),0);
//   // //auto binder2=color(spin(cicc,2),1);
  
//   // // eval(binder1)=8.0;
//   // printf("%d\n",cicc.getStor().nel);
//   printf("ANNA %lg\n",eval(binder1));
//   // cout<<Spacetime::name()<<endl;
//   // cout<<Col::name()<<endl;


// }

int main()
{
  //test_first_vectorizer();
  //test_transpose();
  //test_conj();
  test_isAliasing();
  //test_bind_complicated_expression();
  
  return 0;
}
