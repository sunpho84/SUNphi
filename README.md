# SUNphi: SU(N) on Phi architecture

A library to simulate SU(N) (and similar) theories, with multinode,
multicore, supervectorized architectures in mind, and a narcissistic
name. Hopefully, a modernistic replacement for
[Nissa](https://github.com/sunpho84/nissa) libary.

The key idea is to have a numerical library able to operate on tensor
expressions of arbitrary components, in such a way to be completely
neutral with respect to the internal layout of the object (order of
the components, kind of the expression, underlying data type), and to
enable automatic optimization/parallelization/vectorization of the
expression execution, via an efficient (?) army of metaprogramming
techniques.

Features:

- header-only

- crazy metaprogramming at work, inspired by
  [Eigen](http://eigen.tuxfamily.org/) and
  [Blaze](https://bitbucket.org/blaze-lib/blaze) libraries

- [C++17](https://en.wikipedia.org/wiki/C%2B%2B17) syntax: God bless
  the [constexpr
  if](http://en.cppreference.com/w/cpp/language/if#Constexpr_If)
  statements. By the time I have finished this library, most compiler
  will support it (hopefully...)

- non-shared memory parallelism via MPI (not yet implemented, but this
  will be easy to adapt from Nissa), with plenty of ideas (in my mind)
  on how to automatize caching/scattering

- automatic parallelization of free indices via loop fusion (done) and
  thread pool (...should be easily adaptable/generalizable from Nissa)

- automatic vectorization on the innermost component(s) (via loop
  fusion or splitting, and automatic deduction of the set of vector
  operation appliable to a given vector size... almost all still in my
  mind, so far)

- fully documented with [Doxygen](www.doxygen.org), otherwise I would
  forget immediately the meaning of what I write.

At the core, the library is an engine to build [Smart Expression
Templates](https://arxiv.org/pdf/1104.1729.pdf), applied to vector
carrying an arbitrary number of indices (called "tensor components" in
the following).

Implemented so far: tensor components fusion, reorganization of unary
expressions such as "conj" and "transpose".

Missing: ...a lot of things, almost everything actually! Most
relevantly, MPI, threads, vectors and all useful expressions... But
all of that is sketched and ready to be forgotten.


A simple example of what can be be done so far:

```c++
#include <SUNphi>

using namespaces SUNphi;

// Defines a custom tensor component:
// - myComp is the name of the function that will be used to access to the component
// - MyComp is the name of the component
// - CompSize is the name of the constant variable that will hold the size
// - size (8 here) could be marked as DYNAMIC, so that the size can be specified at runtime
DEFINE_TENS_COMP(myComp,MyComp,CompSize,8);

// A fake copier that (if we were on an AVX-512 enabled machine), would copy rhs to lhs
void AVX512copy(double& lhs,const double &rhs)
{
}

int main()
{
    // Tensor kind: list of tensor components
    // - RwCol is meant to be a "row color" index
    //   (number of colors is specified at configure time)
    // - Compl is a pair, accessed through real and imag, or reim
    //   with either 0 or 1 parameter
    // - Spin is an alias for CnSpin (column spin), every "twinned" (column/row)
    //   tensor component access to the Cn version if not specified
    // - Spacetime is internally marked as a "DYNAMIC" component
    using MyTk=TensKind<RwCol,Compl,CnCol,Spacetime,Spin,MyComp>;
    
    // Tensor type: list of components + fundamental type
    using MyTens=Tens<MyTk,double>;

    // Provided size for dynamic component "Spacetime"
    int vol=16;
    // A pair of tensors
    MyTens ciccio(vol),ciaccio(vol);

    // Example of a nontrivial expression assignement
    spin(transpose(ciccio),1)=spin(adj(conj(ciaccio)),2);

    // Explenation:
    //
    // - "spin" gets a view to a specific entry of the tensor component "Spin",
    //   (1 in this case)
    // - "transpose" swaps row with column indices (and types in the TensKind
    //   signature, see above MyTk) for colour and spin
    // - "conj" returns a conjugated view on the bracketed expression,
    //   so that getting the real part of it does nothing, while getting the
    //   imaginary returns minus it
    // - "adj" transposes and takes the conjugated simultaneously
    //
    // Ta-da! Some magic occurs at assignement:
    //
    // - transposition is moved to lhs (so that reading would be sequential)
    // - the two nested "transposed" annihilates with each other,leaving out (at
    //   compile time) a reference to "ciaccio"
    // - the same occurrs to the conugation, leaving a reference to ciccio
    // - all the most external free indices of the expression are fused, and threadized (soon)
    // - the innermost component is recognized to be vectorizable (on his way)
    // 
    // so that the previous code produces an assembly almost identical to this:

    double* _ciccio=getRawAlignedMem<double>(9216);
    double* _ciaccio=getRawAlignedMem<double>(9216);

    // This is (will) be actually dispatched to a thread pool
    #pragma omp parallel for 
    for(int i=0;i<288;i++)
       AVX512copy(_ciccio[8*(1+4*i)],_ciaccio[8*(2+4*i)]);

    free(_ciccio);
    free(_ciaccio);

    return 0;
}
```
