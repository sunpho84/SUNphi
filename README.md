# SUNphi: SU(N) on Phi architecture

**Note**: internal documentation generated with Doxygen is availale
  [here](https://sunpho84.github.io/SUNphi/html/index.html).

A library to simulate *SU(N)* (and similar) theories, with multinode,
multicore, supervectorized architectures in mind, and a narcissistic
name. Hopefully, a modernistic replacement for
[Nissa](https://github.com/sunpho84/nissa) library.

### Goals

**The key idea** is to have a numerical library able to operate on tensor
expressions of arbitrary components, in such a way to be completely
neutral with respect to the internal layout of the object (order of
the components, kind of the expression, underlying data type), and to
enable automatic optimization/parallelization/vectorization of the
expression execution, via an efficient (?) army of metaprogramming
techniques.

- At the core, the library is an engine to build [Smart Expression
 Templates](https://arxiv.org/pdf/1104.1729.pdf), applied to vector
 carrying an arbitrary number of indices (called "tensor components"
 in the following).

- Flexibility to enable vectorization via *site fusion* or other SIMD
  enabling grouping of the data to operate upon.

- Automatic (or better, *guided*) decomposition of a lattice in
  sublattice, using a generalization of the approach used in Nissa.

- Serialization of classes with support for default initialization and
 automatic skipping with default values; creation of standard bison
 parser via meta-parsing the sources.

- Automatic creation of read or write cache area in tensors, on the
  basis of the operation acting on the tensor itself (as it was done
  with the all-to-all remapper in Nissa) and atomic write access.

### Features

- Header-only (so far)

- Crazy metaprogramming at work, inspired by
  [Eigen](http://eigen.tuxfamily.org/) and
  [Blaze](https://bitbucket.org/blaze-lib/blaze) libraries

- [C++17](https://en.wikipedia.org/wiki/C%2B%2B17) syntax: God bless
  the [constexpr
  if](http://en.cppreference.com/w/cpp/language/if#Constexpr_If)
  statements. By the time I have finished this library, most compiler
  will support it (hopefully...)

- Non-shared memory parallelism via MPI (not yet implemented, but this
  will be easy to adapt from Nissa), with plenty of ideas (in my mind)
  on how to automatize caching/scattering

- Automatic parallelization of free indices via loop fusion (done) and
  thread pool (...should be easily adaptable/generalizable from Nissa)

- Automatic vectorization on the innermost component(s) (via loop
  fusion or splitting, and automatic deduction of the set of vector
  operation appliable to a given vector size... almost all still in my
  mind, so far)

- [Fully
  documented](https://sunpho84.github.io/SUNphi/html/index.html) with
  [Doxygen](www.doxygen.org), otherwise I would forget immediately the
  meaning of what I write.

### Status

Implemented so far: tensor components fusion, reorganization of unary
expressions such as "conj" and "transpose".

Missing: ...a lot of things, almost everything actually! Most
relevantly, MPI, threads, vectors and all useful expressions... But
all of that is sketched and ready to be forgotten.

### Examples

A simple example of what can be be done so far:

```c++
#include <SUNphi>

using namespaces SUNphi;

// Defines a custom tensor component:
//
// - myComp is the name of the function that will be used to access to the component
// - MyComp is the name of the component
// - CompSize is the name of the constant variable that will hold the size
// - size (8 here) could be marked as DYNAMIC, so that the size can be specified at runtime
//
DEFINE_TENS_COMP(myComp,MyComp,CompSize,8);

int main()
{
    // Tensor kind: list of tensor components
    //
    // - RwCol is meant to be a "row color" index
    //   (number of colors is specified at configure time)
    // - Compl is a pair, accessed through real and imag, or reim
    //   with either 0 or 1 parameter
    // - Spin is an alias for CnSpin (column spin), every "twinned" (column/row)
    //   tensor component access to the Cn version if not specified
    // - Spacetime is internally marked as a "DYNAMIC" component
    //
    using MyTk=TensKind<RwCol,Compl,CnCol,Spacetime,Spin,MyComp>;
    
    // Tensor type: list of components + fundamental type
    using MyTens=Tens<MyTk,double>;

    // Provided size for dynamic component "Spacetime"
    int vol=16;
    // A pair of tensors
    MyTens ciccio(vol),ciaccio(vol);

    // Example of a nontrivial expression assignement
    spin(transpose(ciccio),1)=spin(adj(conj(ciaccio)),2);

    return 0;
}
```

Explenation:

- `spin` gets a view to a specific entry of the tensor component `Spin`
  (1 on lhs, 2 on rhs in this example)
- `transpose` swaps row with column TensComp in the `TensKind`
  signature, see above `MyTk`) for `Col` and `Spin`
- `conj` returns a conjugated view on the bracketed expression,
  so that getting the real part of it does returns the real part, while
  getting the imaginary returns minus it
- `adj` transposes and takes the conjugated simultaneously

Ta-da! Some magic occurs at assignement:

- the `adj`-instantiated `transpose` is moved to lhs (this is useful because loops
  are run on free indices of ths, which menas that reading would be sequential)
- the two nested `transpose` annihilates with each other, leaving out (at
  compile time) a reference to "ciaccio"
- the same occurrs to the `conj`, leaving out a reference to ciccio
- all the most external free indices of the expression are fused, and threadized (soon)
- the innermost component is recognized to be vectorizable (feature on his way)

The previous code produces an assembly almost identical to that
generated by this dumb c++98 code:

```c++
#include <cstdlib>

// A fake copier that (if we were on an AVX-512 enabled machine),
// would copy rhs to lhs with some intrinsic
void AVX512copy(double& lhs,const double &rhs)
{
}

int main()
{
  // Allocates
  int nel=9216;
  double *_ciccio,*_ciaccio;
  posix_memalign((void**)&_ciccio,64,sizeof(double)*nel);
  posix_memalign((void**)&_ciaccio,64,sizeof(double)*nel);
  
  // This is (will) be actually dispatched to a thread pool
#pragma omp parallel for
  for(int i=0;i<288;i++)
    AVX512copy(_ciccio[8*(1+4*i)],_ciaccio[8*(2+4*i)]);
  
  free(_ciccio);
  free(_ciaccio);
  
  return 0;
}
```
