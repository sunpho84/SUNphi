#ifndef _PARTITIONER_HPP
#define _PARTITIONER_HPP

/// \file Partitioner.hpp
///
/// \brief Split a grid into subgrids

#include <functional>
#include <set>

#include <lattice/Grid.hpp>

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
    
  public:
    
    /// Reference to tje actual grid
    const void* actualGrid;
    
    /// Unset the volume
    void unSetVol()
      const
    {
      volume.reset();
    }
    
    /// Set the volume to \c v
    bool setVol(const Vol& v=0)
      const
    {
      if(volumeIsSet() and volume!=v)
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
    
    /// Get the volume
    const Vol& getVol()
      const
    {
      return
	volume.value();
    }
    
    /// Name for debug purpose
    const char* name;
    
    /// Function to invocate setting the sides of a grid
    const std::function<void(const Sides&)> setSidesOfGrid;
    
    /// Check if the volume is set
    bool volumeIsSet()
      const
    {
      return
	static_cast<bool>(volume);
    }
    
    /// Constructor taking the grid and type-erasing it
    template <typename G>
    ShadowGrid(G&& actualGrid,
	       const char* name) :
      actualGrid(&actualGrid),
      name(name),
      setSidesOfGrid([&actualGrid](const Sides& sides)
		     {
		       actualGrid.setSides(sides);
		     })
    {
      const auto actualVolume=
	actualGrid.volume();
      
      if(actualVolume)
	volume=
	  actualVolume;
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
      relatives[CHILD2];
    
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
	if(relatives[iRel]->volumeIsSet())
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
    
  public:
    
    /// Determines if the volume has an upper bound coming from direct
    /// fathers or if the volume is set
    bool volumeIsBound(const SGrid* grid)
      const
    {
      if(grid->volumeIsSet())
	return
	  true;
      
      for(auto& father : this->getAllFathersOf(grid))
	if(father->volumeIsSet())
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
	if(father->volumeIsSet())
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
	  if(child->volumeIsSet())
	    {
	      SCOPE_INDENT(runLog);
	      
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
	      
	      if(not isOk)
		RUNLOG<<"Father "<<(*father)->name<<" has volume "<<fatherVol<<" not divisible by his child "<<grid->name<<" "<<vol;
	      
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
	      
	      if(not isOk)
		RUNLOG<<"Child "<<(*child)->name<<" has volume "<<vol<<" not dividing his father "<<grid->name<<" "<<vol;
	      
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
    template <typename C,
	      typename F>
    auto getFatherVolDeducer(const SGrid* grid,
			     const SGrid* child1,
			     const SGrid* child2,
			     C&& checker,
			     F&& nextInstruction)
    {
      return
	[grid,
	 child1,
	 child2,
	 checker,
	 nextInstruction]()
	{
	  /// Deduced volume
	  const auto vol=
	    child1->getVol()*child2->getVol();
	  
	  RUNLOG<<"Setting volume of father "<<grid->name<<" to "<<vol<<" deduced by its children "<<child1->name<<" and "<<child2->name;
	  grid->setVol(vol);
	  
	  if(checker())
	    nextInstruction();
	  
	  grid->unSetVol();
	};
    }
    
    /// Returns a function which tries to set the volume deducing it from father and sister
    template <typename C,
	      typename F>
    auto getChildVolDeducer(const SGrid* grid,
			    const SGrid* father,
			    const SGrid* sister,
			    C&& checker,
			    F&& nextInstruction)
    {
      return
	[grid,
	 father,
	 sister,
	 checker,
	 nextInstruction]()
	{
	  /// Deduced volume
	  const auto vol=
	    father->getVol()/sister->getVol();
	  
	  RUNLOG<<"Setting volume of children "<<grid->name<<" to "<<vol<<" deduced by its father "<<father->name<<" and "<<sister->name;
	  grid->setVol(vol);
	  
	  if(checker())
	   nextInstruction();
	  
	  grid->unSetVol();
	};
    }
    
    /// Returns a list of all enforsable relations
    auto listAllEnforsableRelations()
      const
    {
      Vector<std::pair<const PartRel*,int>> list;
      
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
	if(grid.volumeIsSet()==f)
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
    
    using Instruction=
       std::function<void(void)>;
    
    /// Returns the compiled partitioner
    Instruction compile(const Instruction& fun,
			Vector<ScopeDoer<Instruction>>& cleanup)
    {
      if(getAllGridsWithUnfixedVol().size()==0)
	return
	  fun;
      
      /// List of enforcible relations
      const auto enforcibleRelations=
	listAllEnforsableRelations();
      
      RUNLOG<<"Number of enforcible relations: "<<enforcibleRelations.size();
      
      if(enforcibleRelations.size())
	{
	  /// Enforcible relation and index of relative to enforce
	  auto& rw=
	    enforcibleRelations.front();
	  
	  /// Relation to be enforced
	  auto& r=
	    rw.first->relatives;
	  
	  /// Index of relative to enforce
	  const int w=
	    rw.second;
	  
	  RUNLOG<<"Could enforce connection between "<<r[0]->name<<" and its children "<<r[1]->name<<" and "<<r[2]->name<<" to element "<<w;
	  
	  r[w]->setVol();
	  cleanup.emplace_back(getVolUnsetter(r[w]));
	  
	  auto checker=
	    getGridChecker(r[w]);
	  
	  /// Next instruction
	  auto nextInstruction=
	    compile(fun,cleanup);
	  
	  if(w==0)
	    return
	      getFatherVolDeducer(r[0],r[1],r[2],checker,nextInstruction);
	  else
	    return
	      getChildVolDeducer(r[w],r[0],r[3-w],checker,nextInstruction);
	}
      
      return
	[]()
	{
	  CRASH<<"Trying to execute an ill-formed program";
	};
    }
    
    std::function<void(void)> compile(const std::function<void(void)>& fun)
    {
      Vector<ScopeDoer<Instruction>> cleanup;
      
      return
	compile(fun,cleanup);
    }
    
  };
}

#endif
