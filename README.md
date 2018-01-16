# SUNphi
SU(N) on Phi architecture


A simple example

```
#include <SUNphi>

using namespaces std;

int main()
{
      using MyTk=TensKind<RwCol,Spin,Compl,CnCol,Spacetime>;
      using MyTens=Tens<MyTk,double>;
      
      int vol=16;
      MyTens ciccio(vol),ciaccio(vol);
      
      spin(transpose(ciccio),1)=spin(adj(conj(ciaccio)),1);
      
      return 0;
}
```