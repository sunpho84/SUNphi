# SUNphi
SU(N) on Phi architecture


A simple example:

```c++
#include <SUNphi>

using namespaces SUNphi;

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

The previous code produces an assembly almost identical to this:

```c++
int main()
{
      double* ciccio=new double[1152];
      double* ciaccio=new double[1152];
      
      for(int i=0;i<3;i++)
         for(int j=0;j<96;j++)
            ciccio[j+96*(1+4*i)]=ciaccio[j+96*(1+4*i)];

      delete[] ciccio;
      delete[] ciaccio;

      return 0;
}
```
