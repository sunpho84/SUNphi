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
    
    /// Reference to tje actual grid
    const void* actualGrid;
    
    /// Volume of the grid, stored as optional value
    ///
    /// Storing as optional allows to check if the value is set or not
    /// in a coincise way
    std::optional<Vol> volume;
    
    /// Set the volume to \c v
    bool setVol(const Vol& v)
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
    
    /// Unset the volume
    void unSetVol()
    {
      volume.reset();
    }
    
  public:
    
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
    ShadowGrid(G&& actualGrid) :
      actualGrid(&actualGrid),
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
    using Grid=
     ShadowGrid<NDims,Coord,Idx>;
    
    /// Pointer to the grid to be partitioned
    const Grid* father;
    
    /// Pointer to the dividing grid
    const Grid* child1;
    
    /// Pointer to the divisor grid
    const Grid* child2;
  };
  
  /// Partition a set of grids enforcing to obay to certain rules
  template <int NDims=4,                                   // Number of dimensions
	    typename Coord=int32_t,                        // Type of coordinate values
	    typename Idx=int64_t>                          // Type of index of points
  class Partitioner
  {
    /// Short alias for a Partition relation
    using PartRel=
      PartitioningRelation<NDims,Coord,Idx>;
    
    /// Short alias of the ShadowGrid
    using Grid=
     ShadowGrid<NDims,Coord,Idx>;
    
    /// List of relation to be satisfied between all the grid to be partitioned
    std::set<PartRel> partitionRelations;
    
    /// List of grids to be partitioned
    std::set<Grid> grids;
    
  public:
    
    /// Adds a grid to the list of grids to be partitioned
    template <typename G>
    const Grid* addGrid(G&& grid)
    {
      return
	&*grids.emplace(grid).first;
    }
  };
}

/*
  const int nDims=
    4;
  
  SimpleGrid<> glb(nDims,"glb");
  
  SimpleGrid<> eo(nDims,"eo");
  
  SimpleGrid<> glbEo(nDims,"glbEo");
  
  SimpleGrid<> proc(nDims,"proc");
  
  SimpleGrid<> locEo(nDims,"locEo");
  
  SimpleGrid<> doubleSIMD(nDims,"doubleSIMD");
  
  SimpleGrid<> doubleVirtEo(nDims,"doubleVirtEo");
  
  SimpleGrid<> floatSIMD(nDims,"floatSIMD");
  
  SimpleGrid<> floatVirtEo(nDims,"floatVirtEo");
  
  if(not glb.setVol(48*48*48*96))
    CRASH<<"Could not set glb vol";
  
  if(not floatSIMD.setVol(16))
    CRASH<<"Could not set float SIMD";
  
  if(not doubleSIMD.setVol(8))
    CRASH<<"Could not set double SIMD";
  
  if(not eo.setVol(2))
    CRASH<<"Could not set eo";
  
  if(not proc.setVol(128))
    CRASH<<"Could not set proc vol";
  RUNLOG<<"Proc vol: "<<proc.getVol();
  
  Partitioner<> p;
  
  p.add(glb,eo,glbEo).
    add(glbEo,proc,locEo).
    add(locEo,floatSIMD,floatVirtEo).
    add(locEo,doubleSIMD,doubleVirtEo);
  
  p.printDot();
  
  for(auto& it : p.getAllNodesWithUnfixedVol())
    RUNLOG<<it->grid->name<<", volume is bound: "<<p.volumeIsBound(it);
  
  auto c=
    p.compile();
  
  int i=
    0;
  
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
	  
	  if(c[i].second())
	    {
	      RUNLOG<<"Success, moving to next instruction";
	      
	      i++;
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
	      
	  if(c[i].first())
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
  while(i>=0 and i<c.size());
} 

template <typename I=int64_t>
class SimpleGrid
{
public:
  /// Volume of the grid
  I vol;
  
  /// Sides of the grid
  Vector<I> sides;
  
  /// Name of the partitioning
  const std::string name;
  
  /// Returns the volume
  const I& getVol()
    const
  {
    return
      vol;
  }
  
  /// Determines if the volume is set
  bool volumeIsSet()
    const
  {
    return
      vol!=0;
  }
  
  /// Constuct a simple grid
  SimpleGrid(const int nDim,             ///< Number of dimension
	     const std::string& name) :  ///< Name of the grid
        vol(0),
	sides(nDim,0),
	name(name)
	
  {
  }
  
};



  auto gridsOfNodes(const Vector<const Node*>& listOfNodes)
    const
  {
    Vector<SimpleGrid<>*> listOfGrids(listOfNodes.size());
    
    std::transform(listOfNodes.begin(),listOfNodes.end(),listOfGrids.begin(),[](const Node* node){return node->grid;});
    
    return
      listOfGrids;
  }
  
  /////////////////////////////////////////////////////////////////
  
  auto getChildrenToBeChecked(const Node* node)
    const
  {
    Vector<const Node*> childrenToBeChecked;
    
    for(auto& childPair : this->getAllChildrenOf(node))
      for(auto& child : {childPair.first,childPair.second})
	if(child->grid->volumeIsSet())
	  childrenToBeChecked.push_back(child);
    
    return
      childrenToBeChecked;
  }
  
  auto getNodeChecker(const Node* node)
    const
  {
    return
      [grid=node->grid,
       fathersToBeChecked=gridsOfNodes(getFathersToBeChecked(node)),
       childrenToBeChecked=gridsOfNodes(getChildrenToBeChecked(node))]()
      {
	const I& vol=
	  grid->getVol();
	
	for(auto& father : fathersToBeChecked)
	  if(father->getVol()%vol!=0)
	    return
	      false;
	
	for(auto& child : childrenToBeChecked)
	  if(vol%child->getVol()!=0)
	    return
	      false;
	
	return
	  true;
      };
  }
  
  auto getVolSetter(const Node* node,
		    const I& vol)
  {
    return
      [grid=node->grid,
       vol,
       checker=getNodeChecker(node)]()
      {
	RUNLOG<<"Setting volume of "<<grid->name<<" to "<<vol;
	grid->setVol(vol);
	
	return
	  checker();
      };
  }
  
  auto getFatherVolDeducer(const Node* node,
			   const Node* child1,
			   const Node* child2)
  {
    return
      [grid=node->grid,
       child1grid=child1->grid,
       child2grid=child2->grid,
       checker=getNodeChecker(node)]()
      {
	const I vol=
	  child1grid->getVol()*child2grid->getVol();
	
	RUNLOG<<"Setting volume of father "<<grid->name<<" to "<<vol<<" deduced by its children "<<child1grid->name<<" and "<<child2grid->name;
	grid->setVol(vol);
	
	return
	  checker();
      };
  }
  
  auto getChildVolDeducer(const Node* node,
			   const Node* father,
			   const Node* sister)
  {
    return
      [grid=node->grid,
       fatherGrid=father->grid,
       sisterGrid=sister->grid,
       checker=getNodeChecker(node)]()
      {
	const I vol=
	  fatherGrid->getVol()/sisterGrid->getVol();
	
	RUNLOG<<"Setting volume of children "<<grid->name<<" to "<<vol<<" deduced by its father "<<fatherGrid->name<<" and "<<sisterGrid->name;
	grid->setVol(vol);
	
	return
	  checker();
      };
  }

  //     // 	{
  //     // 	const I& childVol=
  //     // 	  child->getVol();
	
  //     // 	if(childVol and v%childVol!=0)
  //     // 	  return
  //     // 	    false;
  //     // }
    
    
  //   bool wentWell=
  //     true;
    
  //   auto fatherSister=
  //     fatherSisters.begin();
    
  //   while(wentWell and fatherSister!=fatherSisters.end())
  //     {
  // 	PartitioningLevel& father=
  // 	  *fatherSister->first;
	
  // 	PartitioningLevel& sister=
  // 	  *fatherSister->second;
	
  // 	if(father.getVol())
  // 	  {
  // 	    RUNLOG<<"Father "<<father.name<<" has already set volume";
  // 	    if(not sister.getVol())
  // 	      {
  // 		RUNLOG<<"Sister "<<sister.name<<" has no set volume";
		
  // 		SCOPE_INDENT(runLog);
  // 		wentWell&=
  // 		  sister.setVol(father.getVol()/v);
  // 	      }
  // 	    else
  // 	      RUNLOG<<"Also sister "<<sister.name<<" had set volume";
  // 	  }
  // 	else
  // 	  {
  // 	    RUNLOG<<"Father "<<father.name<<" has no set volume";
  // 	    if(sister.getVol())
  // 	      {
  // 		RUNLOG<<"Sister "<<sister.name<<" had it, computing father "<<father.name<<" volume";
		
  // 		SCOPE_INDENT(runLog);
  // 		wentWell&=
  // 		  father.setVol(v*sister.getVol());
  // 	      }
  // 	    else
  // 	      RUNLOG<<"Neither sister "<<sister.name<<" had set volume, cannot fix anything";
  // 	  }
	
  // 	fatherSister++;
  //     }
    
  //   if(fatherSisters.size()==0)
  //     RUNLOG<<grid.name<<" has no father";
    
  //   if(auto childPair=children.begin();wentWell)
  //     while(wentWell and childPair!=children.end())
  // 	{
  // 	  auto& firstChild=
  // 	    *(childPair->first);
	  
  // 	  auto& secondChild=
  // 	    *(childPair->second);
	  
  // 	  const I firstVol=
  // 	    firstChild.getVol();
	  
  // 	  const I secondVol=
  // 	    secondChild.getVol();
	  
  // 	  if(firstVol and not secondVol)
  // 	    {
  // 	      RUNLOG<<"Second child "<<secondChild.name<<" has no set volume";
	      
  // 	      SCOPE_INDENT(runLog);
	      
  // 	      wentWell&=
  // 		secondChild.setVol(v/firstVol);
  // 	    }
	  
  // 	  if(secondVol and not firstVol)
  // 	    {
  // 	      RUNLOG<<"First child "<<firstChild.name<<" has no set volume";
	      
  // 	      SCOPE_INDENT(runLog);
	      
  // 	      wentWell&=
  // 		childPair->first->setVol(v/secondVol);
  // 	    }
	  
  // 	  childPair++;
  // 	}
    
  //   if(wentWell)
  //     RUNLOG<<"Volume of "<<grid.name<<" set to "<<v;
  //   else
  //     {
  // 	RUNLOG<<"Could not set volume of "<<grid.name<<" to "<<v;
  // 	grid.vol=
  // 	  0;
  //     }
    
  //   return
  //     wentWell;
  // }
  
  // auto fixedVolSetter(const Node* node,
  // 		      const I& vol)
  // {
  //   return
  //     [this,node,&vol]()
  //     {
  // 	if(not volSetter(node,vol)())
  // 	  CRASH<<"Cannot fix volume of grid "<<node->name();
  // 	else
  // 	  RUNLOG<<"Fixed volume of grid "<<node->name()<<" to"<<vol;
	
  // 	return
  // 	  true;
  //     };
  // }
  
  auto getVolUnsetter(const Node* node)
  {
    return
       [node]()
       {
	 node->grid->unSetVol();
	 
	 return
	   false;
       };
  }
  
public:
  
  /// Determines if the volume has an upper bound coming from direct
  /// fathers or if the volume is set
  bool volumeIsBound(const Node* node)
    const
  {
    if(node->grid->volumeIsSet())
      return
	true;
    
    for(auto& father : this->getAllFathersOf(node))
      if(father->grid->volumeIsSet())
	return
	  true;
    
    return
      false;
  }
  
  Partitioner& add(SimpleGrid<I>& father,
		   SimpleGrid<I>& child1,
		   SimpleGrid<I>& child2)
  {
    if(&father==&child1)
      CRASH<<"Father "<<father.name<<" must be different from child1 "<<child1.name;
    
    if(&father==&child2)
      CRASH<<"Father "<<father.name<<" must be different from child2 "<<child2.name;
    
    if(&child1==&child2)
      CRASH<<"Child1 "<<child1.name<<" must be different from child2 "<<child2.name;
    
    connections.insert({addNode(father),addNode(child1),addNode(child2)});
    
    return
      *this;
  }
  
  void printDot()
    const
  {
    
    std::map<const Node*,int> iChild;
    
    for(auto& g : connections)
      {
	int& i=
	  iChild[g[0]];
	
	runLog()<<g[0]->name()<<" -> "<<g[0]->name()<<i;
	for(int j=1;j<=2;j++)
	  runLog()<<g[0]->name()<<i<<" -> "<<g[j]->name();
	
	i++;
      }
  }
  
  Vector<const Node*> getAllFathersOf(const Node* node)
    const
  {
    Vector<const Node*> res;
    
    for(auto& c: connections)
      for(int j=1;j<=2;j++)
	if(c[j]==node)
	  res.push_back(c[0]);
    
    return
      res;
  }
  
  auto listAllEnforsableConnections()
    const
  {
    Vector<std::pair<const Connection*,int>> list;
    
    for(auto& c : connections)
      {
	int lastNotFixed=
	  0;
	
	int nFixed=
	  0;
	
	for(int i=0;i<3;i++)
	  if(c[i]->grid->volumeIsSet())
	    nFixed++;
	  else
	    lastNotFixed=
	      i;
	
	if(nFixed==2)
	  list.emplace_back(&c,lastNotFixed);
      }
    
    return
      list;
  }
  
  Vector<std::pair<const Node*,const Node*>> getAllChildrenOf(const Node* node)
    const
  {
    Vector<std::pair<const Node*,const Node*>> res;
    
    for(auto& c: connections)
      if(c[0]==node)
	res.push_back({c[1],c[2]});
    
    return
      res;
  }
  
  /// List all elements of the partitioning with unfixed nodes
  template <bool F>
  Vector<const Node*> getAllNodesWithFixedOrUnfixedVol()
    const
  {
    /// List to be returned
    Vector<const Node*> list;
    
    for(auto& node : nodes)
      if(node.grid->volumeIsSet()==F)
	list.push_back(&node);
    
    return
      list;
  }
  
  Vector<const Node*> getAllNodesWithFixedVol()
    const
  {
    return
      getAllNodesWithFixedOrUnfixedVol<true>();
  }
  
  Vector<const Node*> getAllNodesWithUnfixedVol()
    const
  {
    return
      getAllNodesWithFixedOrUnfixedVol<false>();
  }
  
  using Action=
	std::function<bool(void)>;
  
  using Move=
	std::pair<Action,Action>;
  
  Vector<Move> compile()
  {
    constexpr I MARK_FIXED=
      -10;
    
    Vector<Move> res;
    
    int nNodesToBeExplored=
      nodes.size();
    
    for(auto& node : getAllNodesWithFixedVol())
      {
	res.push_back({getVolUnsetter(node),getVolSetter(node,node->grid->getVol())});
	
	nNodesToBeExplored--;
      }
    
    Vector<decltype(getScopeChangeVar(*(I*){nullptr},I{}))> listOfSetVolume;
    
    bool somethingDone;
    
    do
      {
	somethingDone=
	  false;
	
	const auto enforcibleConnections=
	  listAllEnforsableConnections();
	
	if(enforcibleConnections.size())
	  {
	    somethingDone=
	      true;
	    
	    auto& cw=
	      enforcibleConnections.front();
	    
	    auto& c=
	      *cw.first;
	    
	    const int w=
	      cw.second;
	    
	    RUNLOG<<"Could enforce connection between "<<c[0]->name()<<" and its children "<<c[1]->name()<<" and "<<c[2]->name()<<" to element "<<w;
	    
	    if(w==0)
	      res.push_back({getVolUnsetter(c[0]),getFatherVolDeducer(c[0],c[1],c[2])});
	    else
	      res.push_back({getVolUnsetter(c[w]),getChildVolDeducer(c[w],c[0],c[3-w])});
	    
	    listOfSetVolume.emplace_back(getScopeChangeVar(c[w]->grid->vol,MARK_FIXED));
	  }
      }
    while(somethingDone);
    
    // while(nNodesToBeExplored)
    //   {
    // 	for(auto& node : getAllNodesWithUnfixedVol())
    // 	  if()
    //   }
    
    return
      res;
  }
  
  // /// Visit all nodes executing the passed function on current node
  // /// and appending to the list of visited
  // ///
  // /// Internal implementation
  // template <typename F>
  // void visitAll(Vector<const Node*>& visited,   ///< List of visited nodes
  // 		const F& fun)                   ///< Function to be run on this node
  // {
  //   visited.push_back(this);
    
  //   fun(this);
    
  //   for(auto& fatherSister : fatherSisters)
  //     if(auto& father=fatherSister.first;not visited.isContained(father))
  // 	father->visitAll(visited,fun);
    
  //   for(auto& childPair : children)
  //     for(auto& child : {childPair.first,childPair.second})
  // 	if(not visited.isContained(child))
  // 	  child->visitAll(visited,fun);
  // }
  
};

 
*/

#endif
