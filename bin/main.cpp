#include <SUNphi.hpp>

#include <array>
#include <iostream>

using namespace std;
using namespace SUNphi;

#define WHEREAMY				\
    cout<<"///////////////////////////////////////////////////////////////// "<<__PRETTY_FUNCTION__<<"/////////////////////////////////////////////////////////////////"<<endl


// void test_first_vectorizer()
// {
//   int a=MyTk::firstVectorizingComp<double>;
//   cout<<a<<endl;
// }

// void test_transpose()
// {
//   WHEREAMY;
  
//   using Su3Tens=Tens<TensKind<RwCol,CnCol>,double>;
  
//   Su3Tens t;
  
//   for(int rw_c=0;rw_c<NCOL;rw_c++)
//     for(int cn_c=0;cn_c<NCOL;cn_c++)
//       rwCol(cnCol(transpose(t),cn_c),rw_c)=cn_c;
  
//   for(int rw_c=0;rw_c<NCOL;rw_c++)
//     for(int cn_c=0;cn_c<NCOL;cn_c++)
//       cout<<cn_c<<" "<<rwCol(cnCol(t,cn_c),rw_c)<<endl;
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
  WHEREAMY;
  
  STATIC_ASSERT_DUPLICATED_CALL_REMOVER(conj,Tens<TensKind<Compl>,double>);
  STATIC_ASSERT_DUPLICATED_CALL_REMOVER(transpose,Tens<TensKind<RwCol,CnCol>,double>);
  STATIC_ASSERT_DUPLICATED_CALL_REMOVER(adj,Tens<TensKind<Compl>,double>);
  STATIC_ASSERT_DUPLICATED_CALL_ABSORBER(wrap,Tens<TensKind<Compl>,double>);
}

// void test_binding()
// {
//   WHEREAMY;
  
//   Tens<TensKind<RwSpin,Compl>,double> cicc;
  
//   reim(spin(cicc,0),0)=1.923424;
  
//   cout<<&spin(reim(cicc,0),0)<<" "<<cicc.getStor()._v<<endl;
//   //cout<<reim(a,0)<<endl;
  
//   //spin(wrap(reim(cicc,0)),1)=1.0;
//   //cout<<"is_lvalue: "<<is_lvalue_reference<decltype()>::value<<endl;;
// }

// void test_isAliasing()
// {
//   using ComplTens=Tens<TensKind<Compl>,double>;
  
//   ComplTens t,u;
  
//   //transpose_bis(t);
  
//   auto a=wrap(Tens<TensKind<Compl>,double>{});
//   auto b=wrap(wrap(Tens<TensKind<Compl>,double>{}));
  
//   cout<< IsSame<RemoveReference<decltype(a)>,RemoveReference<decltype(b)>> <<endl;
//   cout<<IsSame<decltype(a),decltype(b)><<endl;
//   cout<<"t.isAliasing(t): "<<t.isAliasing(t.getStor())<<endl;
//   cout<<"t.isAliasing(u): "<<t.isAliasing(u.getStor())<<endl;
//   cout<<"conj(t).isAliasing(t): "<<conj(t).isAliasing(t.getStor())<<endl;
// }

void test_conj()
{
  WHEREAMY;
  
  using ComplTens=Tens<TensKind<Compl>,double>;
  
  ComplTens t;
  
  real(t)=1.0;
  imag(t)=-3.14598712480;
  
  //auto &c1=t;
  auto c2=conj(t);
  
  printf("im: %lg",reim(c2,0));
  printf("im: %lg",reim(c2,1));
}

// void test_bind_complicated_expression()
// {
//   WHEREAMY;
  
//   Tens<TensKind<Compl>,double> t;
//   auto tr=transpose(wrap(wrap(t)));
//   reim(transpose(tr),0)=1.0;
//   reim(transpose(transpose(Tens<TensKind<Compl>,double>{})),0)=1.0;
//        //const double &e=reim(conj()),0);
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
//   	double &ref=site(reim(spin(col(transpose(transpose(cicc)),ic),id),ri),0);
//   	//printf("%lld %lld\n",(long long int)cicc.get_v()._v,(long long int)&ref);
//   	ref=3.141592352352;
//       }
  
//   auto binder1=site(reim(spin(col(cicc,2),3),1),0);
//   // //auto binder2=color(spin(cicc,2),1);
  
//   // // eval(binder1)=8.0;
//   // printf("%d\n",cicc.getStor().nel);
//   printf("ANNA %lg\n",binder1);
//   // cout<<Spacetime::name()<<endl;
//   // cout<<Col::name()<<endl;
//  }

// void test_assigner()
// {
//   WHEREAMY;
  
//   using MyTk1=TensKind<Col,Spin,Compl>;
//   using MyTens1=Tens<MyTk1,double>;
  
//   using MyTk2=TensKind<Spin,Col,Compl>;
//   using MyTens2=Tens<MyTk2,double>;
  
//   MyTens1 cicc1;
//   MyTens2 cicc2;
  
//   cout<<"cicc1 storage: "<<getStor(cicc1)._v<<endl;
  
//   for(int icol=0;icol<NCOL;icol++)
//     for(int ispin=0;ispin<NSPIN;ispin++)
//       for(int icompl=0;icompl<NCOMPL;icompl++)
//   	col(spin(reim(cicc1,icompl),ispin),icol)=
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
//   // 	  cout<<b<<endl;
//   // 	}
//   // cout<<a<<endl;
  
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
  
//   // cout<<"/////////////////////////////////////////////////////////////////"<<endl;
//   // // for(int icol=0;icol<NCOL;icol++)
//   // //   for(int icompl=0;icompl<NCOMPL;icompl++)
//   // //     {
//   // // 	col(reim(cicc2,icompl),icol).eval()=
//   // // 	  col(reim(conj(cicc1),icompl),icol).eval();
//   // // 	cout<<col(reim(cicc2,icompl),icol).eval()<<
//   // // 	  " "<<col(reim(cicc1,icompl),icol).eval()<<endl;
//   // //     }
//   // // cout<<"/////////////////////////////////////////////////////////////////"<<endl;

//   // auto c1=conj(cicc1);
//   // auto ass=assign(cicc2,c1);
//   //  ass.close();
  
//   // for(int icol=0;icol<NCOL;icol++)
//   //   for(int icompl=0;icompl<NCOMPL;icompl++)
//   //     cout<<col(reim(cicc2,icompl),icol).eval()<<" "<<col(reim(cicc1,icompl),icol).eval()<<endl;
  
// }

int main()
{
  //test_first_vectorizer();
  //test_transpose();
  test_conj();
  //test_isAliasing();
  //test_binding();
  //test_bind_complicated_expression();
  test_duplicated_call_remover();
  //test_assigner();
  
  return 0;
}
