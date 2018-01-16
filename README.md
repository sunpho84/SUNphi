# SUNphi
SU(N) on Phi architecture


A simple example, this code
x
```c++
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

produces a code as if this code has been compiled

```c++
int main()
{
      double* a=new double[1152];
      
      for(int i=0;i<3;i++)
         for(int j=0;j<96;j++)
            ciccio[j+96*(1+4*i)]=ciaccio[j+96*(1+4*i)];
      
      return 0;
}
```
