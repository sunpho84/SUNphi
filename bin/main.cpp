//#define DEBUG_TENS
//#define DEBUG_BINDER
//#define DEBUG_INDEXER
//#define DEBUG_REL_BINDER

#include <SUNphi.hpp>

#include <iostream>

using namespace std;
using namespace SUNphi;

/// Grid to be partitioned
template <typename I=int64_t>
class PartitioningLevel
{
  /// Voluem of the grid
  I vol;
  
  /// Sides of the grid
  Vector<I> sides;
  
  /// Partitioning Level Pair of Pointers
  using PLPP=
    std::pair<PartitioningLevel*,PartitioningLevel*>;
  
  /// List of fathers which are parted by this grid, and sister
  /// (result of the partitioning)
  ///
  /// Sister could be accessed through father search, but it's easier
  /// to provide a shortcut
  Vector<PLPP> fatherSisters;
  
  /// List of children grid partitioning this grid
  Vector<PLPP> children;
  
  /// Visit all nodes executing the passed function on current node
  /// and appending to the list of visited
  ///
  /// Internal implementation
  template <typename F>
  void visitAll(Vector<PartitioningLevel*>& visited,   ///< List of visited nodes
		const F& fun)                          ///< Function to be run on this node
  {
    visited.push_back(this);
    
    fun(this);
    
    for(auto& fatherSister : fatherSisters)
      if(auto& father=fatherSister.first;not visited.isContained(father))
	father->visitAll(visited,fun);
    
    for(auto& childPair : children)
      for(auto& child : {childPair.first,childPair.second})
	if(not visited.isContained(child))
	  child->visitAll(visited,fun);
  }
  
public:
  
  /// Name of the partitioning
  const std::string name;
  
  /// Visit all nodes executing the passed function on current node
  /// and appending to the list of visited
  template <typename F>
  void visitAll(const F& fun)                          ///< Function to be run on this node
  {
    /// List of visited node
    Vector<PartitioningLevel*> visited;
    
    visitAll(visited,fun);
  }
  
  /// List all elements of the partitioning with unfixed nodes
  Vector<PartitioningLevel*> getAllWithUnfixedVol()
  {
    /// List to be returned
    Vector<PartitioningLevel*> list;
    
    visitAll([&list](PartitioningLevel* pl)
	     {
	       if(pl->vol==0)
		 list.push_back(pl);
	     });
    
    return
      list;
  }
  
  /// Determines if the volume is set
  bool volumeIsSet()
    const
  {
    return
      vol!=0;
  }
  
  /// Determines if the volume has an upper bound coming from direct
  /// fathers or if the volume is set
  bool volumeIsBound()
    const
  {
    if(volumeIsSet())
      return
	true;
    
    for(auto& fatherSister : fatherSisters)
      if(auto& father=fatherSister.first;father->volumeIsSet())
	return
	  true;

    return
      false;
  }
  
  /// Constructor
  PartitioningLevel(const int nDim,
		    const std::string& name) :
    vol(0),
    sides(nDim,0),
    name(name)
  {
  }
  
  /// Returns the volume
  const I& getVol()
    const
  {
    return
      vol;
  }
  
  void addFatherSister(PartitioningLevel& f,
		       PartitioningLevel& s)
  {
    fatherSisters.push_back({&f,&s});
    
    f.addChild(*this,s);
  }
  
  void addChild(PartitioningLevel<I>& a,
		PartitioningLevel<I>& b)
  {
    children.push_back({&a,&b});
  }
  
  bool setVol(const I& v)
  {
    if(vol!=0)
      return
	false;
    
    if(vol==v)
      return
	true;
    
    for(auto& fatherSister : fatherSisters)
      {
	PartitioningLevel* father=
	  fatherSister.first;
	
	const I& fathVol=
	  father->getVol();
	
	if(fathVol and fathVol%v!=0)
	  return
	    false;
      }
    
    for(auto& childPair : children)
      for(auto& child : {childPair.first,childPair.second})
	{
	const I& childVol=
	  child->getVol();
	
	if(childVol and v%childVol!=0)
	  return
	    false;
      }
    
    vol=
      v;
    
    bool wentWell=
      true;
    
    auto fatherSister=
      fatherSisters.begin();
    
    while(wentWell and fatherSister!=fatherSisters.end())
      {
	PartitioningLevel& father=
	  *fatherSister->first;
	
	PartitioningLevel& sister=
	  *fatherSister->second;
	
	if(father.getVol())
	  {
	    RUNLOG<<"Father "<<father.name<<" has already set volume";
	    if(not sister.getVol())
	      {
		RUNLOG<<"Sister "<<sister.name<<" has no set volume";
		
		SCOPE_INDENT(runLog);
		wentWell&=
		  sister.setVol(father.getVol()/v);
	      }
	    else
	      RUNLOG<<"Also sister "<<sister.name<<" had set volume";
	  }
	else
	  {
	    RUNLOG<<"Father "<<father.name<<" has no set volume";
	    if(sister.getVol())
	      {
		RUNLOG<<"Sister "<<sister.name<<" had it, computing father "<<father.name<<" volume";
		
		SCOPE_INDENT(runLog);
		wentWell&=
		  father.setVol(v*sister.getVol());
	      }
	    else
	      RUNLOG<<"Neither sister "<<sister.name<<" had set volume, cannot fix anything";
	  }
	
	fatherSister++;
      }
    
    if(fatherSisters.size()==0)
      RUNLOG<<name<<" has no father";
    
    if(auto childPair=children.begin();wentWell)
      while(wentWell and childPair!=children.end())
	{
	  auto& firstChild=
	    *(childPair->first);
	  
	  auto& secondChild=
	    *(childPair->second);
	  
	  const I firstVol=
	    firstChild.getVol();
	  
	  const I secondVol=
	    secondChild.getVol();
	  
	  if(firstVol and not secondVol)
	    {
	      RUNLOG<<"Second child "<<secondChild.name<<" has no set volume";
	      
	      SCOPE_INDENT(runLog);
	      
	      wentWell&=
		secondChild.setVol(v/firstVol);
	    }
	  
	  if(secondVol and not firstVol)
	    {
	      RUNLOG<<"First child "<<firstChild.name<<" has no set volume";
	      
	      SCOPE_INDENT(runLog);
	      
	      wentWell&=
		childPair->first->setVol(v/secondVol);
	    }
	  
	  childPair++;
	}
    
    if(wentWell)
      RUNLOG<<"Volume of "<<name<<" set to "<<v;
    else
      {
	RUNLOG<<"Could not set volume of "<<name<<" to "<<v;
	vol=
	  0;
      }
    
    return
      wentWell;
  }
};

template <typename I=int64_t>
void createPartitioningHierarchy(PartitioningLevel<I>& f,
				 PartitioningLevel<I>& c1,
				 PartitioningLevel<I>& c2)
{
  c1.addFatherSister(f,c2);
  c2.addFatherSister(f,c1);
  
  f.addChild(c1,c2);
}

int main()
{
  Sitmo::Rng rng;
  
  rng.seed(trueRandomGenerator);
  
  //enc.setKey({235425});
  runLog()<<rng.serialize();
  
  MasterRandomGenerator mrg;
  
  RUNLOG<<mrg.serialize();
  std::ostringstream os;
  os<<*static_cast<_MasterRandomGenerator*>(&mrg);
  runLog()<<os.str()<<" "<<_MasterRandomGenerator::word_size;
  
  //test2.deBinarize(test1.binarize());
  
  // runLog()<<"Bin";
  
  // {
  //   SCOPE_INDENT(runLog);
  //   runLog()<<test2;
  // }
  
  // runLog()<<(void*)&(test1.test().a().first[0])<<" "<<(void*)&(test2.test().a().first[0]);
  
  // // (*static_cast<Logger::LoggerLine*>(nullptr))<<(*static_cast<RemRef<SerializeWrapper<double>>*>(nullptr));
  // ciccio=10.0;
  // // decltype(*static_cast<Logger*>(nullptr)<<double{}) &a="";
  
  // std::ostringstream os;
  // os<<std::mt19937_64{};
  // runLog()<<os.str();
  
  // runLog()<<ciccio;
  
  // runLog()<<canPrint<Logger,Serializable<double>>;
  
  // std::get<0>(test.ser)=
  //   10.0;
  
  // runLog()<<test.ser.mappedSize;
  // runLog()<<test.d;
  
  
  // SerializableScalar<Test> test{"test",{}};
  // Serializable<Test2,NO_DEFAULT> t2{"t2"};
  
  // YAML::Emitter out;
  
  // out<<t2.serialize();
  
  // runLog()<<out.c_str();

  // // test().a=11.0;
  // // runLog()<<test().a();
  
  // Serializer ser;
  // ser<<t2;
  
  // DeSerializer deSer(ser.get().c_str());
  
  // deSer>>t2;
  
  // Serializer ser2;
  // ser2<<t2;
  
  // //runLog()<<t2;
  
  // // Binarizer bin=t2.binarize();
  // // runLog()<<bin.size();

  // // t2().ciccio="scompigliato";
  // // t2().test().a=1235;
  // // ser2<<t2;
  // // runLog()<<ser2.get();

  // // 	    t2.deBinarize(bin);
  // // Serializer ser3;
  // // ser3<<t2;
  // // runLog()<<ser3.get();
  
  
  // // double d;
  // // int i;

  // // auto l=serList(d,"d",10.0,
  // // 		 i,"i",10);
  // // runLog()<<((&std::get<0>(std::get<0>(l)))!=&d);
  
  // // SerializableScalar<double> _aref("a",NO_DEFAULT);
  
  // // runLog()<<isSerializableScalar<SerializableScalar<double>>;
  
  return 0;
}
