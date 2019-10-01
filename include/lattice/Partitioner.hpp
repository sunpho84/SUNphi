#ifndef _PARTITIONER_HPP
#define _PARTITIONER_HPP

/// \file Partitioner.hpp
///
/// \brief Split a grid into subgrids

#include <functional>
#include <set>

#include <lattice/Grid.hpp>

#include <debug/Warning.hpp>
#include <math/Partition.hpp>
#include <metaprogramming/UniversalReferences.hpp>

namespace SUNphi
{
  /// A generic grid to type-erase the actual grid
  ///
  /// While looping on all possible realization of a grid, there is
  /// little point in keeping updated the hash-table and other
  /// features of the target grid. We make a dedicated, simplified
  /// version of the grid, and work with it. We could make the Grid
  /// a dynamic polymorhic class but this way we risk to introduce
  /// severe performances degradation when calling any method
  /// (e.g. neighbors search)
  template <int NDims,                             // Number of dimensions
	    typename Coord,                        // Type of coordinate values
	    typename Idx>                          // Type of index of points
  class ShadowGrid
  {
    PROVIDE_COORDS_TYPES;
    
    /// Volume of the grid, stored as optional value
    ///
    /// Storing as optional allows to check if the value is set or not
    /// in a coincise way
    std::optional<Vol> mutable volume;
    
    /// Sdes of the grid
    std::optional<Coords> mutable sides;
    
  public:
    
    /// Reference to the actual grid
    const void* actualGrid;
    
    /// Unset the volume
    void unSetVol()
      const
    {
      volume.reset();
    }
    
    /// Unset the sides
    void unSetSides()
      const
    {
      sides.reset();
    }
    
    /// Set the volume to \c v
    bool setVol(const Vol& v)
      const
    {
      if(volIsSet() and volume!=v)
	return
	  false;
      else
	{
	  volume=
	    v;
	  
	  return
	    true;
	}
    }
    
    /// Set the sides to \c s
    bool setSides(const Sides& s)
      const
    {
      if(sidesAreSet() and sides!=s)
	return
	  false;
      else
	{
	  sides=
	    s;
	  
	  setVol(getVolFromSides());
	  
	  return
	    true;
	}
    }
    
    /// Get the volume
    const Vol& getVol()
      const
    {
      return
	volume.value();
    }
    
    /// Get the sides
    const Sides& getSides()
      const
    {
      return
	sides.value();
    }
    
    /// Get a given side
    const Side& getSide(int mu)
      const
    {
      return
	getSides()[mu];
    }
    
    /// Get the volume from sides
    const Vol getVolFromSides()
      const
    {
      /// Returned value
      Vol v=
	1;
      
      for(int mu=0;mu<getSides().size();mu++)
	v*=
	  getSide(mu);
      
      return
	v;
    }
    
    /// Name for debug purpose
    const char* name;
    
    /// Function to invocate setting the sides of the actual grid
    const std::function<void(const Sides&)> setSidesOfActualGrid;
    
    /// Check if the volume is set
    bool volIsSet()
      const
    {
      return
	static_cast<bool>(volume);
    }
    
    /// Check if the sides are set
    bool sidesAreSet()
      const
    {
      return
	static_cast<bool>(sides);
    }
    
    /// Constructor taking the grid and type-erasing it
    template <typename G>
    ShadowGrid(G&& actualGrid,
	       const char* name) :
      actualGrid(&actualGrid),
      name(name),
      setSidesOfActualGrid([&actualGrid](const Sides& sides)
		     {
		       actualGrid.setSides(sides);
		     })
    {
      /// Reads the sides
      bool actualGridHasSetVolume=
	true;
      
      actualGrid.forAllDims([&](int mu)
			     {
			       actualGridHasSetVolume&=
				 (actualGrid.side(mu)!=0);
			     });
      
      if(actualGridHasSetVolume)
	setSides(actualGrid.sides());
      else
	{
	  /// Reads the volume
	  const auto actualVolume=
	    actualGrid.volume();
	  
	  if(actualVolume)
	    setVol(actualVolume);
	}
    }
    
    /// Comparison operator
    bool operator<(const ShadowGrid& oth)
      const
    {
      return
  	actualGrid<oth.actualGrid;
    }
  };
  
  /// Type to describe a partition relation between grid to be partitioned
  template <int NDims,                             // Number of dimensions
	    typename Coord,                        // Type of coordinate values
	    typename Idx>                          // Type of index of points
  class PartitioningRelation
  {
  public:
    
    /// Short alias of the ShadowGrid
    using SGrid=
     ShadowGrid<NDims,Coord,Idx>;
    
    /// List of relatives
    std::array<const SGrid*,3> relatives;
    
    /// Position of father, child1 and child2 in the list of relatives
    enum{FATHER,CHILD1,CHILD2,NO_RELATIVE};
    
    /// Pointer to the grid to be partitioned
    const SGrid*& father=
      relatives[FATHER];
    
    /// Pointer to the dividing grid
    const SGrid*& child1=
      relatives[CHILD1];
    
    /// Pointer to the divisor grid
    const SGrid*& child2=
      relatives[CHILD2];
    
    /// Returns whether we can deduce something:
    ///
    /// - father and child1 is set, so child2 can be deduced
    /// - father and child2 is set, so child1 can be deduced
    /// - child1 and child2 is set, so father can be deduced
    int getDeducible()
      const
    {
      /// Take trace of the last relative not fixed
      int lastNotFixedRel=
	NO_RELATIVE;
      
      /// Count how many relatives are fixed
      int nFixed=
	0;
      
      for(int iRel=0;iRel<3;iRel++)
	if(relatives[iRel]->volIsSet())
	  nFixed++;
	  else
	    lastNotFixedRel=
	      iRel;
	
      if(nFixed==2)
	return
	  lastNotFixedRel;
      else
	return
	  NO_RELATIVE;
    }
    
    /// Constructor
    PartitioningRelation(const SGrid* father,
			 const SGrid* child1,
			 const SGrid* child2)
      : relatives{father,child1,child2}
    {
      if(&father==&child1)
	CRASH<<"Father "<<father->name<<" must be different from child1 "<<child1->name;
      
      if(&father==&child2)
	CRASH<<"Father "<<father->name<<" must be different from child2 "<<child2->name;
      
      if(&child1==&child2)
	CRASH<<"Child1 "<<child1->name<<" must be different from child2 "<<child2->name;
    }
    
    /// Comparison operator
    bool operator<(const PartitioningRelation& oth)
      const
    {
      return
	relatives<oth.relatives;
    }
  };
  
  /// A program made of a set of pair of instructions
  class PartitioningProgram
  {
    /// Action to be performed in the list
    using Action=
	std::function<bool(void)>;
    
    /// Pair of actions to be performed forward or backward
    using Move=
	std::pair<Action,Action>;
    
    /// List of instrcutions to be executed
    Vector<Move> instructions;
    
    /// Data used for loops
    Vector<int> data;
    
  public:
    
    /// Adds an instruction into the program
    void add(Action&& fw,   ///< Instruction to be executed when advancing
	     Action&& bw)   ///< Instruction to be exectued when rewinding
    {
      instructions.emplace_back(fw,bw);
    }
    
    /// Runs the program
    void exec()
    {
      /// Index of the current instruction
      int i=
	0;
      
      /// Current direction of motion
      BACK_FORW dir=
	FORW;
      
      do
	{
	  RUNLOG<<"Instruction "<<i;
	  
	  SCOPE_INDENT(runLog);
	  
	  if(dir==FORW)
	    {
	      RUNLOG<<"Moving forward";
	      
	      SCOPE_INDENT(runLog);
	      
	      if(instructions[i].second())
		{
		  RUNLOG<<"Success, moving to next instruction";
		  
		  i++;
		  
		  if(i==instructions.size())
		    {
		      RUNLOG<<"Arrived to the end of the program, rewinding";
		      
		      i--;
		      
		      dir=
			BACK;
		    }
		}
	      else
		{
		  RUNLOG<<"Failed, reverting move backward";
		  
		  dir=
		    BACK;
		}
	    }
	  else
	    {
	      RUNLOG<<"Moving backward";
	      
	      SCOPE_INDENT(runLog);
	      
	      if(instructions[i].first())
		{
		  RUNLOG<<"Success, reverting move forward";
		  
		  dir=
		    FORW;
		}
	      else
		{
		  RUNLOG<<"Failed, moving to previous instruction";
		  
		  i--;
		}
	    }
	}
      while(i>=0);
    }
  };
  
  /// Partition a set of grids enforcing to obay to certain rules
  template <int NDims=4,                                   // Number of dimensions
	    typename Coord=int32_t,                        // Type of coordinate values
	    typename Idx=int64_t>                          // Type of index of points
  class Partitioner
  {
    PROVIDE_COORDS_TYPES;
    
    /// Short alias for a Partition relation
    using PartRel=
      PartitioningRelation<NDims,Coord,Idx>;
    
    /// Short alias of the ShadowGrid
    using SGrid=
     ShadowGrid<NDims,Coord,Idx>;
    
    /// Instruction to be exectuted
    using Instruction=
       std::function<void(void)>;
    
    /// List of relation to be satisfied between all the grid to be partitioned
    std::set<PartRel> partitionRelations;
    
    /// List of grids to be partitioned
    std::set<SGrid> grids;
    
    /// Find the shadow of an actual grid
    template <typename G>
    const SGrid* findShadowOfGrid(G&& actualGrid)
      const
    {
      /// Result of research
      auto res=
	std::find_if(grids.begin(),grids.end(),[&actualGrid](const SGrid& grid){return grid.actualGrid==&actualGrid;});
      
      if(res!=grids.end())
	return
	  &*res;
      else
	return
	  nullptr;
    }
    
    /// Pretend to set a volume for a grid
    void pretendToSetVol(const SGrid* grid,
			 Vector<ScopeDoer<Instruction>>& cleanup)
    {
      /// Dummy volue to set
      constexpr Vol dummyVol=
	0;
      
      grid->setVol(dummyVol);
      cleanup.emplace_back(getVolUnsetter(grid));
    }
    
  public:
    
    /// Determines if the volume has an upper bound coming from direct
    /// fathers or if the volume is set
    bool volIsBound(const SGrid* grid)
      const
    {
      if(grid->volIsSet())
	return
	  true;
      
      for(auto& father : this->getAllFathersOf(grid))
	if(father->volIsSet())
	  return
	    true;
      
      return
	false;
    }
    
    /// Adds a grid to the list of grids to be partitioned
    template <typename G>
    const SGrid* addGrid(G&& grid,
			const char* name)
    {
      return
	&*grids.emplace(grid,name).first;
    }
    
    /// Adds a partition relation
    template <typename GF,
	      typename GC1,
	      typename GC2>
    const PartRel* addPartitionRelation(GF&& father,
					GC1&& child1,
					GC2&& child2)
    {
      /// Find father
      const SGrid* sFather=
	findShadowOfGrid(father);
      
      /// Find child1
      const SGrid* sChild1=
	findShadowOfGrid(child1);
      
      /// Find child2
      const SGrid* sChild2=
	findShadowOfGrid(child2);
      
      if(sFather==nullptr)
	CRASH<<"Father not found";
      
      if(sChild1==nullptr)
	CRASH<<"Child1 not found";
      
      if(sChild2==nullptr)
	CRASH<<"Child2 not found";
      
      return
	&*partitionRelations.emplace(sFather,sChild1,sChild2).first;
    }
    
    /// Gets the list of fathers to be checked
    auto getFathersToBeChecked(const SGrid* grid)
      const
    {
      /// Returned list
      Vector<const SGrid*> fathersToBeChecked;
      
      for(auto& father : this->getAllFathersOf(grid))
	if(father->volIsSet())
	    fathersToBeChecked.push_back(father);
      
      return
	fathersToBeChecked;
    }
    
    /// Gets the list of children to be checked
    auto getChildrenToBeChecked(const SGrid* grid)
      const
    {
      /// Returned list
      Vector<const SGrid*> childrenToBeChecked;
      
      for(auto& childPair : this->getAllChildrenOf(grid))
	for(auto& child : {childPair.first,childPair.second})
	  if(child->volIsSet())
	    {
	      RUNLOG<<child->name<<" volume is set, will need to be checked";
	      
	      childrenToBeChecked.push_back(child);
	    }
      
      return
	childrenToBeChecked;
    }
    
    /// Returns a function which check the validity of a grid and move to next instruction
    auto getGridChecker(const SGrid* grid)
      const
    {
      SCOPE_INDENT(runLog);
      RUNLOG<<"Creating the checker for "<<grid->name;
      
      return
	[grid,
	 fathersToBeChecked=getFathersToBeChecked(grid),
	 childrenToBeChecked=getChildrenToBeChecked(grid)]()
	{
	  SCOPE_INDENT(runLog);
	  RUNLOG<<"Checking grid "<<grid->name;
	  
	  /// Current grid volume
	  const Vol& vol=
	    grid->getVol();
	  
	  /// Keep track if constraints are satisfied
	  bool isOk=
	    true;
	  
	  /// Looping on fathers
	  auto father=
	    fathersToBeChecked.begin();
	  
	  while(isOk and father!=fathersToBeChecked.end())
	    {
	      /// Volume of father
	      const auto fatherVol=
		(*father)->getVol();
	      
	      isOk&=
		(fatherVol%vol==0);
	      
	      RUNLOG<<"Father "<<(*father)->name<<" has volume "<<fatherVol<<(isOk?" ":" not")<<" divisible by his child "<<grid->name<<" "<<vol;
	      
	      father++;
	    }
	  
	  /// Looping on children
	  auto child=
	    childrenToBeChecked.begin();
	  
	  while(isOk and child!=childrenToBeChecked.end())
	    {
	      /// Volume of child
	      const auto childVol=
		(*child)->getVol();
	      
	      isOk&=
		(vol%childVol==0);
	      
	      RUNLOG<<"Child "<<(*child)->name<<" has volume "<<vol<<(isOk?" ":" not")<<" dividing his father "<<grid->name<<" "<<vol;
	      
	      child++;
	    }
	  
	  return
	    isOk;
	};
    }
    
    // /// Returns a function which tries to set the volume
    // template <typename V>
    // auto getVolSetter(const SGrid* grid,
    // 		      const V& vol)
    // {
    //   return
    //   [grid,
    //    vol,
    //    checker=getGridChecker(grid)]()
    //   {
    // 	RUNLOG<<"Setting volume of "<<grid->name<<" to "<<vol;
    // 	grid->setVol(vol);
	
    // 	return
    // 	  checker();
    //   };
    // }
    
    /// Returns a function which unset the volume
    auto getVolUnsetter(const SGrid* grid)
    {
      return
	[grid]()
	{
	  grid->unSetVol();
	};
    }
    
    /// Returns a function which tries to set the volume deducing it from children
    template <typename F>
    auto getFatherVolDeducer(const SGrid* grid,
			     const SGrid* child1,
			     const SGrid* child2,
			     F&& nextInstruction)
    {
      return
	[grid,
	 child1,
	 child2,
	 nextInstruction]()
	{
	  /// Deduced volume
	  const auto vol=
	    child1->getVol()*child2->getVol();
	  
	  SCOPE_INDENT(runLog);
	  RUNLOG<<"Setting volume of father "<<grid->name<<" to "<<vol<<" deduced by its children "<<child1->name<<" ("<<child1->getVol()<<") and "<<child2->name<<" ("<<child2->getVol()<<")";
	  
	  grid->setVol(vol);
	  
	  nextInstruction();
	  
	  grid->unSetVol();
	};
    }
    
    /// Returns a function which tries to set the volume deducing it from father and sister
    template <typename F>
    auto getChildVolDeducer(const SGrid* grid,
			    const SGrid* father,
			    const SGrid* sister,
			    F&& nextInstruction)
    {
      return
	[grid,
	 father,
	 sister,
	 nextInstruction]()
	{
	  /// Deduced volume
	  const Vol vol=
	    father->getVol()/sister->getVol();
	  
	  SCOPE_INDENT(runLog);
	  RUNLOG<<"Setting volume of children "<<grid->name<<" to "<<vol<<" deduced by its father "<<father->name<<" ("<<father->getVol()<<") and "<<sister->name<<" ("<<sister->getVol()<<")";
	  grid->setVol(vol);
	  nextInstruction();
	  
	  grid->unSetVol();
	};
    }
    
    /// Type to host an enforcceable relation
    using EnforceableRel=
      std::pair<const PartRel*,int>;
    
    /// Returns a list of all enforceable relations
    auto listAllEnforceableRelations()
      const
    {
      Vector<EnforceableRel> list;
      
      for(auto& p : partitionRelations)
	{
	  const int iDeducible=
	    p.getDeducible();
	  
	  if(iDeducible!=PartRel::NO_RELATIVE)
	    list.emplace_back(&p,iDeducible);
	}
      
      return
	list;
    }
    
    /// Gets all fathers of a given grid
    Vector<const SGrid*> getAllFathersOf(const SGrid* grid)
      const
    {
      /// Returned list
      Vector<const SGrid*> res;
      
      for(auto& p: partitionRelations)
	for(int j=1;j<=2;j++)
	  if(p.relatives[j]==grid)
	    res.push_back(p.relatives[PartRel::FATHER]);
      
      return
	res;
    }
    
    /// Gets all children of a given grid
    Vector<std::pair<const SGrid*,const SGrid*>> getAllChildrenOf(const SGrid* grid)
      const
    {
      /// Result
      Vector<std::pair<const SGrid*,const SGrid*>> res;
      
      for(auto& p : partitionRelations)
	if(p.relatives[PartRel::FATHER]==grid)
	  res.push_back({p.relatives[PartRel::CHILD1],p.relatives[PartRel::CHILD2]});
      
      return
	res;
    }
    
    /// List all grids of the partitioning with fixed or unfixed volume, according to the parameter
    Vector<const SGrid*> getAllGridsWithFixedOrUnfixedVol(const bool f)
      const
    {
      /// List to be returned
      Vector<const SGrid*> list;
      
      for(auto& grid : grids)
	if(grid.volIsSet()==f)
	  list.push_back(&grid);
      
      return
	list;
    }
    
    /// List all grids of the partitioning with fixed volume
    Vector<const SGrid*> getAllGridsWithFixedVol()
      const
    {
      return
	getAllGridsWithFixedOrUnfixedVol(true);
    }
    
    /// List all grids of the partitioning with unfixed volume
    Vector<const SGrid*> getAllGridsWithUnfixedVol()
      const
    {
      return
	getAllGridsWithFixedOrUnfixedVol(false);
    }
    
    /// Print the partition relation in a Dot format
    void printDot()
      const
    {
      /// Labels of all partitions
      std::map<const SGrid*,int> iChild;
      
      for(auto& g : partitionRelations)
	{
	  /// Label of the father node
	  int& i=
	    iChild[g.father];
	  
	  RUNLOG<<g.father->name()<<" -> "<<g.father->name()<<i;
	  
	  for(auto& child : {g.child1,g.child2})
	    RUNLOG<<g.father->name()<<i<<" -> "<<child->name();
	
	i++;
      }
    }
    
    /// Return the greatest divisor of all the volumes
    static Vol greatestDivisorOfVolumes(const Vector<const SGrid*>& list)
    {
      /// Greatest common divisor to be returned
      Vol gcd=
	    0;
      
      for(auto& f : list)
	gcd=
	  greatestCommonDivisor(gcd,f->getVol());
      
      return
	gcd;
    }
    
    /// Return the least common multiple of all the volumes
    static Vol leastCommonMultipleOfVolumes(const Vector<const SGrid*>& list)
    {
      /// Least common multiple to be returned
      Vol lcm=
	1;
      
      for(auto& c : list)
	lcm=
	  leastCommonMultiple(lcm,c->getVol());
      
      return
	lcm;
    }
    
    /// Return a conditional executer
    template <typename C,
	      typename F>
    Instruction getConditionalExecuter(C&& checker,
				       F&& fun)
    {
      return
	[checker,
	 fun]()
	{
	  if(checker())
	    fun();
	};
    }
    
    /// Returns a function that enforce a given relation
    template <typename Fun>
    Instruction getRelationEnforcer(const EnforceableRel& enfRel,
				    Vector<ScopeDoer<Instruction>>& cleanup,
				    Fun fun)
    {
      /// Relation to be enforced
      auto& r=
	enfRel.first->relatives;
      
      /// Index of relative to enforce
      const int w=
	enfRel.second;
      
      RUNLOG<<"Could enforce connection between "<<r[0]->name<<" and its children "<<r[1]->name<<" and "<<r[2]->name<<" to element "<<w;
      
      pretendToSetVol(r[w],cleanup);
      
      /// Checker to be created before proceeding in the compilation
      auto checker=
	getGridChecker(r[w]);
      
      /// Next instruction
      auto nextInstruction=
	getConditionalExecuter(checker,compile(fun,cleanup));
      
      if(w==0)
	return
	  getFatherVolDeducer(r[0],r[1],r[2],nextInstruction);
      else
	return
	  getChildVolDeducer(r[w],r[0],r[3-w],nextInstruction);
    }
    
    /// Returns a function which loops on all possible volume of \c g
    Instruction getVolumeLooper(const SGrid* g,
				const Instruction& fun,
				Vector<ScopeDoer<Instruction>>& cleanup)
    {
      SCOPE_INDENT(runLog);
      RUNLOG<<"Grid "<<g->name<<" can be looped";
      
      /// Precompute the list of fathers to check
      Vector<const SGrid*> fathersList;
      for(auto& f : getAllFathersOf(g))
	if(f->volIsSet())
	  fathersList.push_back(f);
      
      /// Precompute the list of children to check
      Vector<const SGrid*> childrenList;
      for(auto& c : getAllChildrenOf(g))
	for(auto& cp : {c.first,c.second})
	  if(cp->volIsSet())
	    childrenList.push_back(cp);
      
      pretendToSetVol(g,cleanup);
      
      /// Next instruction
      auto nextInstruction=
	compile(fun,cleanup);
      
      /// Returned instruction
      auto instruction=
	[g,
	 fathersList,
	 childrenList,
	 nextInstruction]()
	{
	  SCOPE_INDENT(runLog);
	  RUNLOG<<"Looping on "<<g->name;
	  
	  /// Greatest divisor of all fathers'volume
	  Vol gcdFatherVol=
	    greatestDivisorOfVolumes(fathersList);
	  
	  RUNLOG<<"Volume must divide "<<gcdFatherVol;
	  
	  /// Least common multiple of all children's volume
	  Vol lcmChildrenVol=
	    leastCommonMultipleOfVolumes(childrenList);
	  
	  RUNLOG<<"Volume must be divisible by "<<lcmChildrenVol;
	  
	  if(gcdFatherVol%lcmChildrenVol==0)
	    {
	      /// Volume to be looped is the quotient between fathers and children
	      const Vol volToPart=
		gcdFatherVol/lcmChildrenVol;
	      
	      RUNLOG<<"VolToPart: "<<volToPart;
	      
	      loopOnAllSubmultiplesOf(volToPart,
				      [g,
				       nextInstruction,
				       lcmChildrenVol]
				      (const Vol& v)
				      {
					/// The volume to be assigned is the product with the children
					const Vol actualVol=
					  v*lcmChildrenVol;
					
					g->setVol(actualVol);
					
					SCOPE_INDENT(runLog);
					RUNLOG<<"Grid "<<g->name<<" set to "<<actualVol;
					nextInstruction();
					
					g->unSetVol();
				      });
	    }
	};
      
      return
	instruction;
    }
    
    /// Returns the compiled partitioner
    Instruction compile(const Instruction& fun,                   ///< Function to be executed at inner instruction
			Vector<ScopeDoer<Instruction>>& cleanup)  ///< Collection of tasks to cleanup after compilation ends
    {
      /// List of all unfixed volume
      auto unfixedVolGridList=
	getAllGridsWithUnfixedVol();
      
      if(unfixedVolGridList.size()==0)
	return
	  fun;
      
      /////////////////////////////////////////////////////////////////
      
      /// List of enforceable relations
      const auto enforceableRelations=
	listAllEnforceableRelations();
      
      RUNLOG<<"Number of enforceable relations: "<<enforceableRelations.size();
      
      if(enforceableRelations.size())
	return
	  getRelationEnforcer(enforceableRelations.front(),cleanup,fun);
      
      /////////////////////////////////////////////////////////////////
      
      for(auto& g : unfixedVolGridList)
	if(volIsBound(g))
	  return
	    getVolumeLooper(g,fun,cleanup);
      
      /////////////////////////////////////////////////////////////////
      
      WARNING<<"The program cannot be run";
      
      return
	[]()
	{
	  CRASH<<"Trying to execute an ill-formed program";
	};
    }
    
    /// Compile the partitioner
    Instruction compile(const Instruction& fun)
    {
      /// Contains all quantity to be cleaned after compilation
      Vector<ScopeDoer<Instruction>> cleanup;
      
      return
	compile(fun,cleanup);
    }
  };
}

#endif
