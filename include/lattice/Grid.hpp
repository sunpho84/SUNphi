#ifndef _GRID_HPP
#define _GRID_HPP

/// \file Grid.hpp
///
/// \brief Defines hypercubic cartesian grids
///
/// An hypercubic grid with number of dimensions fixed at compile
/// time. Periodic boundary conditions are imposed at all faces and
/// lexicographic indexing with first-neighbours connectivity is
/// embedded. The coordinates od points and neighbors can be hashed,
/// deciding at compile time.
///
/// Given the number of dimensions, the two orientations are provided,
/// such that the 2*nDims neighbors are stored alternating first all
/// backwards, thenforward directions.

#include <array>
#include <vector>

#include <metaprogramming/CRTP.hpp>
#include <system/Debug.hpp>
#include <utility/Bits.hpp>

/// Provide the type Coords
#define PROVIDE_COORDS				\
  /*! Type to hold sizes, coordinate, etc */	\
    typedef std::array<Coord,NDims> Coords

/// Flag to enable Grid debug
#define GRID_DEBUG \
  true

namespace SUNphi
{
  /// Value to identifiy orientations
  enum Orientation{BW=0,FW=1};
  
  /// Offset to move \c BW or \c FW
  constexpr int moveOffset[2]=
    {-1,+1};
  
  /// Constant asserting to hash
  static constexpr bool HASHED=
    true;
  
  /// Constant asserting not to hash
  static constexpr bool NOT_HASHED=
    false;
  
  /////////////////////////////////////////////////////////////////
  
  /// Hashable properties of a \c Grid
  ///
  /// Forward implementation
  template <typename T,      // External type inheriting from
	    int NDims,       // Number of dimensions
	    typename Coord,  // Type of coordinate values
	    typename Idx,    // Type of index of points
	    bool Hashing>    // Store or not tables
  class GridHashable;
  
  /// Hashable properties of a \c Grid
  ///
  /// Hashing version
  template <typename T,      // External type inheriting from
	    int NDims,       // Number of dimensions
	    typename Coord,  // Type of coordinate values
	    typename Idx>    // Type of index of points
  class GridHashable<T,
		     NDims,
		     Coord,
		     Idx,
		     HASHED>
  {
    
    PROVIDE_COORDS;
    
    /// Type to hold the neighbors
    typedef std::array<Coord,2*NDims> Neigh;
    
    /// Hashed coords of all points
    std::vector<Coords> coordsOfPointsHashTable;
    
    /// Hashed neighbors
    std::vector<Neigh> neighsOfPointsHashTable;
    
    /// Fills the required hashtable with the function
    template <typename Tb,  // Type of the hashtable
	      typename F>   // Type of the function
    void fillVolumeHashTable(Tb& tb, ///< Hashtable to fill
			     F f)    ///< function to call
    {
      /// Proxy volume
      const Idx volume=
	CRTP_CAST.volume();
      
      // Check hashability
      if(volume>tb.max_size())
	CRASH("Cannot hash a volume of",volume);
      
      // Resize the hash table
      tb.resize(volume);
      
      // Fill the table
      CRTP_CAST.forAllPoints([&](Idx i)
			     {
			       tb[i]=
				 f(i);
			     });
    }
    
    /// Set the hash table of coordinates of all points
    void fillCoordsOfPointsHashTables()
    {
      fillVolumeHashTable(coordsOfPointsHashTable,
			  [&](Idx i)
			  {
			    return
			      CRTP_CAST.computeCoordsOfPoint(i);
			  });
    }
    
    /// Set the hash table of neighbors
    void fillNeighsOfPointsHashTables()
    {
      fillVolumeHashTable(neighsOfPointsHashTable,
			  [&](Idx i)
			  {
			    /// Neighbors to store
			    Neigh out;
			    
			    // Loop on all oriented direction
			    CRTP_CAST.forAllOriDirs([&](int oriDir)
						    {
						      out[oriDir]=
							CRTP_CAST.computeNeighOfPoint(i,oriDir);
						    });
			    
			    return out;
			  });
    }
    
  public:
    
    PROVIDE_CRTP_CAST_OPERATOR(T);
    
    /// Fill all the HashTables
    void fillHashTables()
    {
      fillCoordsOfPointsHashTables();
      fillNeighsOfPointsHashTables();
    }
    
    /////////////////////////////////////////////////////////////////
    
    /// Get the coords of given point
    const Coords& coordsOfPoint(Idx i) const
    {
      CRTP_CAST.assertPointIsInRange(i);
      
      return coordsOfPointsHashTable[i];
    }
    
    /// Return the neighbor in the given oriented dir
    Idx neighOfPoint(const Idx i,
		     const int oriDir) const
    {
      CRTP_CAST.assertPointIsInRange(i);
      CRTP_CAST.assertOriDirIsInRange(oriDir);
      
      return neighsOfPointsHashTable[i][oriDir];
    }
    
    /// Flag asserting that hashing
    static constexpr char isHashing[]=
      "Hashing";
    
  };
  
  /////////////////////////////////////////////////////////////////
  
  /// Hashable properties of a \c Grid
  ///
  /// Not hashing version
  template <typename T,      // External type inheriting from
	    int NDims,       // Number of dimensions
	    typename Coord,  // Type of coordinate values
	    typename Idx>    // Type of index of points
  class GridHashable<T,
		     NDims,
		     Coord,
		     Idx,
		     NOT_HASHED>
  {
    
    PROVIDE_COORDS;
    
  public:
    
    PROVIDE_CRTP_CAST_OPERATOR(T);
    
    /// Get the coords of given point, computing it
    Coords coordsOfPoint(Idx i) const
    {
      return CRTP_CAST.computeCoordsOfPoint(i);
    }
    
    /// Return the neighbor in the given oriented dir, computing it
    Idx neighOfPoint(const Idx i,
		     const int oriDir) const
    {
      return CRTP_CAST.computeNeighOfPoint(i,oriDir);
    }
    
    /// Flag asserting not hashing
    static constexpr char isHashing[]=
      "Not Hashing";
    
  };
  
  /////////////////////////////////////////////////////////////////
  
  /// A grid of points spanning an hypercubic grid
  template <int NDims=4,              // Number of dimensions
	    typename Coord=int32_t,   // Type of coordinate values
	    typename Idx=int64_t,     // Type of index of points
	    bool Hashing=HASHED>      // Store or not tables
  class Grid :
    public GridHashable<Grid<NDims,Coord,Idx,Hashing>,
			NDims,
			Coord,
			Idx,
			Hashing>
  {
    PROVIDE_COORDS;
    
  private:
    
    /// Side of the grid
    Coords _sides;
    
    /// Volume of the grid
    Idx _volume;
    
    /// Set the volume, calling computing routine
    void setVolume()
    {
      /// Output volume, initially 1
      _volume=1;
      
      // Loop on all dimension, taking product
      forAllDims([&](int mu)
		{
		  _volume*=side(mu);
		});
    }
    
  public:
    
    /// Number of dimensions
    static constexpr int nDims=
      NDims;
    
    /// Number of oriented directions
    static constexpr int nOriDirs=
      2*NDims;
    
    /// Get the volume
    const Idx& volume() const
    {
      return _volume;
    }
    
    /// Get the given side
    Coord side(const int mu) const
    {
      return _sides[mu];
    }
    
    /// Check that a given point is in range
    void assertPointIsInRange(const Idx& i) const
    {
      if constexpr(GRID_DEBUG)
	if(i<0 or i>=volume())
	  CRASH("Cannot access to element",i);
    }
    
    /// Check that a given oriented dir is in range
    void assertOriDirIsInRange(const int& oriDir) const
    {
      if constexpr(GRID_DEBUG)
        if(oriDir<0 or oriDir>=2*NDims)
	  CRASH("Cannot use oriented dir",oriDir,"out of range [0,]",2*NDims);
    }
    
    /// Check that a given set of coords are in range
    void assertCoordsAreInRange(const Coords& cs) const
    {
      if constexpr(GRID_DEBUG)
        forAllDims([&](int mu)
		  {
		    /// Maximal value
		    const Coord& m=
		      CRTP_CAST.side(mu);
		    
		    /// Coord mu value
		    const Coord& c=
		      cs[mu];
		    
		    if(c<0 or c>=m)
		      CRASH("Cannot have dimension",mu,"equal to",c,", negative or larger than",m);
		  });
    }
    
    /// Loop on all dimension calling the passed function
    template <typename F>            // Type of the function
    static void forAllDims(F&& f)   ///< Function to be called
    {
      for(int mu=0;mu<nDims;mu++)
	f(mu);
    }
    
    /// Loop on all oriented directions calling the passed function
    template <typename F>                // Type of the function
    static void forAllOriDirs(F&& f)   ///< Function to be called
    {
      for(int oriDir=0;oriDir<nOriDirs;oriDir++)
	f(oriDir);
    }
    
    /// Loop on all points calling the passed function
    template <typename F>        // Type of the function
    void forAllPoints(F&& f)   ///< Function to be called
    {
      for(Idx i=0;i<volume();i++)
	f(i);
    }
    
    /// Orientation of an oriented directions
    static Orientation oriOfOriDir(const int oriDir)
    {
      Orientation out;
      
      // Check smallest bit
      if(bitOf(oriDir,0))
	out=FW;
      else
	out=BW;
      
      return out;
    }
    
    /// Dimension of an oriented directions
    int dimOfOriDir(const int oriDir) const
    {
      // Divide by two
      return oriDir>>1;
    }
    
    /// Oriented direction given orientation and dimension
    int oriDirOfOriAndDim(const Orientation ori,const int dim) const
    {
      return ori+dim*2;
    }
    
    /// Set the sides and trigger the volume change
    void setSides(const Coords& extSides)
    {
      _sides=
	extSides;
      
      setVolume();
      
      if constexpr(Hashing)
	this->fillHashTables();
    }
    
    /// Compte the coordinate of point i
    Coords computeCoordsOfPoint(Idx i) const
    {
      assertPointIsInRange(i);
      
      /// Result
      Coords c;
      
      for(int mu=nDims-1;mu>=0;mu--)
	{
	  /// Dividend, corresponding to the \c mu side length
	  const Coord& d=
	    side(mu);
	  
	  /// Quozient, corresponding to the index of the remaining \cnDims-1 components
	  const Idx q=i/d;
	  
	  /// Remainder, corresponding to the coordinate
	  const Coord r=i-d*q;
	  
	  // Store the component
	  c[mu]=r;
	  
	  // Iterate on the remaining nDims-1 components
	  i=q;
	}
      
      return c;
    }
    
    /// Compute the point of given coords
    Idx pointOfCoords(const Coords& cs) const ///< Coordinates of the point
    {
      assertCoordsAreInRange(cs);
      
      /// Returned point
      Idx out=
	0;
      
      forAllDims([&](int mu)
		{
		  /// Grid side
		  const Coord& s=
		    CRTP_CAST.side(mu);
		  
		  // Increment the coordinate
		  out=
		    out*s+cs[mu];
		});
      
      return out;
    }
    
    /// Returns the coordinates shifted in the asked direction
    ///
    /// Periodic boundary conditions are always assumed
    Coords shiftedCoords(const Coords& in,const int oriDir,const int amount=1) const
    {
      /// Returned coordinates
      Coords out;
      
      /// Orientation
      const int ori=
    	oriOfOriDir(oriDir);
      
      /// Dimension to shift
      const int shiftMu=
    	dimOfOriDir(oriDir);
      
      /// Offset to add
      const int offset=
    	(side(shiftMu)+moveOffset[ori])*amount;
      
      forAllDims([&](int mu)
    		 {
    		   if(mu!=shiftMu)
    		     out[mu]=in[mu];
    		   else
    		     out[mu]=(in[mu]+offset)%side(mu);
    		 });
      
      return out;
    }
    
    /// Compte the neighbor in the oriented direction oriDir of point i
    Idx computeNeighOfPoint(const Idx i,             ///< Point
			       const int oriDir) const  ///< Oriented direction
    {
      assertPointIsInRange(i);
      assertOriDirIsInRange(oriDir);
      
      return pointOfCoords(shiftedCoords(this->coordsOfPoint(i),oriDir));
    }
    
    /// Construct from sides
    Grid(const Coords& sides)
    {
      setSides(sides);
    }
    
  };
  
  /// Deduction guide for bracket list
  template <int NDims,
	    typename Coord>
  Grid(const Coord(&sides)[NDims])
    -> Grid<NDims,int>;
}

#undef PROVIDE_COORDS

#endif
