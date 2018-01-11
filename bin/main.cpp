#include <SUNphi.hpp>

using namespace std;
using namespace SUNphi;

int main()
{
  using MyTk=TensKind<Col,Spin>;
  using MyTens=Tens<MyTk,double>;
  
  MyTens cicc;
  for(int icol=0;icol<NCOL;icol++)
    for(int ispin=0;ispin<NSPIN;ispin++)
      col(spin(cicc,ispin),icol)=
  	  ispin+NSPIN*icol;
  
  double a=0.0;
  for(int icol=0;icol<NCOL;icol++)
    for(int ispin=0;ispin<NSPIN;ispin++)
      a+=col(spin(cicc,ispin),icol);
  
  cout<<a<<endl;
  
  return 0;
}
