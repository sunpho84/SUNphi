# SUNphi: SU(N) on Phi architecture

**Note**: internal documentation generated with Doxygen is available
  [here](https://sunpho84.github.io/SUNphi/html/index.html).

A library to simulate *SU(N)* (and similar) theories, with multinode,
multicore, supervectorized architectures in mind, and a narcissistic
name. Hopefully, a modernistic replacement for
[Nissa](https://github.com/sunpho84/nissa) library.

### Why a new independent library for Lattice QCD (and beyond)

- We can shape it for other precise needs

- We know what we implement (and we can trust it... maybe)

- Consolidate experience with other past libraries, avoiding some
  now-recognized pitfalls

- Use a modern programming language allows for further future
  developments, and simplify mainteinment (provided that good practice
  is adopted while developing it, and a lot of effort is dedicated in
  documenting)

- We have onsiderable workforce amount availbale if we gather the
  force currently involved in
  [Nissa](https://github.com/sunpho84/nissa),
  [tmLQCD](https://github.com/etmc/tmLQCD),
  [DDalphaAMG](https://github.com/sbacchio/DDalphaAMG),
  [CVC](https://github.com/marcuspetschlies/cvc), and a few other
  friend-developers

- Glory, fame, success! More probably, blood, chains and dust...

### Main strategy line

- Be modern ![really
  modern](http://afflictor.com/wp-content/uploads/2015/05/imgRetroFuturism1-2.jpg)

- If in doubt, take as a reference Grid, and do differently. Even
  better, do the opposite

- Get inspiration by available libraries, mix and stir with other
  codes and try to get the best out of them.

### Goals

**The key idea** is to have a numerical library able to operate on
tensor expressions of arbitrary components, in such a way to be
**completely neutral** with respect to the internal layout of the object
(order of the components, kind of the expression, underlying data
type), and to enable automatic
optimization/parallelization/vectorization of the expression
execution, via an efficient (?) army of metaprogramming techniques.

- At the core, the library is an engine to build [Smart Expression
 Templates](https://arxiv.org/pdf/1104.1729.pdf), applied to vector
 carrying an arbitrary number of indices (called "tensor components"
 in the following).

- Flexibility to enable vectorization via *site fusion* or other
  SIMD-enabling grouping of the data, automatically decuced.

- Automatic (or better, *guided*) decomposition of a lattice in
  sublattice, using a generalization of the approach used in Nissa.

- Serialization of classes with support for default initialization and
 automatic skipping with default values; creation of standard bison
 parser via meta-parsing the sources.

- Automatic creation of read or write cache area in tensors, on the
  basis of the operation acting on the tensor itself (as it was done
  with the all-to-all remapper in Nissa) and atomic write access to
  ensure cache coherece across nodes.

- Automatic creation of compound non-local operations, combining
  simple ones (e.g. covariant shift).

### Format and tools used/envisaged

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

- Continuous Integration with
  [Travis](https://travis-ci.org/sunpho84/SUNphi).

### Implementation

- Construction of a syntactic tree of `Tensorial Expressions`
  (originally I had named them TEx, but I realized this was really a
  nasty name and diverted to SmET, reflecting the Smart Expression
  Template nature), e.g. `a*b` defines an node of the class
  `Multiplier(a,b)`, as `a*b+c` defines an nested nodes
  `Summer(Multiplier(a,b),c)`

- Simplifcation/restructuration of the syntatic tree at compile time,
  e.g. (a very simple but important example) on supported
  architectures `Summer(Multiplier(a,b),c)` would be cast into
  `SummerMultiplier(c,a,b)`

- This is achieved through the usage of [`Universal
  References`](https://isocpp.org/blog/2012/11/universal-references-in-c11-scott-meyers)
  to support arbitrary subnodes and
  [`SFINAE`](http://en.cppreference.com/w/cpp/language/sfinae) to
  enable template specialization of each node constructor

- Another related tool, inspired by Eigen, is that each nodes come
  with a cost calculation estimator, so that e.g. the double matrix
  product `a*b*c` can be split into two subproducts with a temporary
  storage `d=b*c`. Certainly more interestingly,
  `CovShift(CovShift(a,X),Y)` could be run with or without temporary
  storage, depending on the lattice partitioning selected, etc

- Incidentally this allows also an automatic calculator of the
  computational cost, and of performances substained in the
  calculation of an expression

- Another important aspect is the aliasing checker (inspired by
  Blaze), allowing to take a straight assigner of buffered one if at
  compile/runtime an alias with rhs is detected

- Each node holds a `TensorKind`, collection of all the `Tensorial
  Components`, e.g. `Spin`, `Color`, `Site`, `Complex`, etc. Each
  `Tensorial Component` represents a fixed or dynamic size, named
  index component of an expression.  Furthermore a fundamental type
  (e.g. double, single, half, or quad) is associated to each
  expression
  
- Each node add, removes or modify the Tensorial Components, enabling
  or not vectorization/fusion of the components

- The position of a tensorial component with respect to the others is
  free, but reflects the internal order in which the index runs to
  determine the storage

- This way an expression acting on a given Tensorial Component can
  treat an arbitary expression with an arbitray `Tensorial Kind`,
  irrespectively to the presence of other components, and to its
  position. To make thing clear with an example: once `CovShift` is
  defined to act on a Tensor Expression provided it comes with a space
  and color index, it would work on a spincolor lattice field, a
  colorspin, a spincolorspin etc

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
    // - Compl is a pair, accessed through real and imag functions, or reim
    //   with either 0 or 1 parameter
    // - Spin is an alias for CnSpin (column spin)
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

**Explenation**:

- `spin` gets a view to a specific entry of the tensor component `Spin`
  (1 on lhs, 2 on rhs in this example)
- `transpose` swaps row with column TensComp in the `TensKind`
  signature, see above `MyTk`) for `Col` and `Spin`
- `conj` returns a conjugated view on the bracketed expression,
  so that getting the real part of it does returns the real part, while
  getting the imaginary returns minus it
- `adj` transposes and takes the conjugated simultaneously

**Ta-da!** Some magic occurs at assignement:

- the `adj`-instantiated `transpose` is moved to lhs (this is useful because loops
  are run on free indices of ths, which menas that reading would be sequential)
- the two nested `transpose` annihilates with each other, leaving out (at
  compile time) a reference to "ciaccio"
- the same occurrs to the `conj`, leaving out a reference to ciccio
- all the most external free indices of the expression are fused, and threadized (soon)
- the innermost component is recognized to be vectorizable (this feature is on his way to arrive)

The previous code produces an assembly almost identical to that
generated by this dumb c++98 code:

```c++
#include <cstdlib>

// A fake copier that (if we were on an AVX-512 enabled machine),
// would copy rhs to lhs with some intrinsic
void AVX512copy(double& lhs,const double& rhs)
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
