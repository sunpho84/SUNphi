#ifndef _GRID_HPP
#define _GRID_HPP

/// \file Grid.hpp
///
/// \brief Defines hypercubic cartesian grids
///
/// An hypercubic grid with number of dimensions fixed at compile
/// time. Periodic boundary conditions are imposed at all faces and
/// lexicographic indexing with first-neighbours connectivity is
/// embedded. The coordinates of points and neighbors can be hashed,
/// deciding at compile time.
///
/// Given the number of dimensions, the two orientations are provided,
/// such that the 2*nDims neighbors are stored putting first all
/// backwards, then forward directions.

#include <array>
#include <vector>

#include <debug/Crash.hpp>
#include <ios/Logger.hpp>
#include <math/Arithmetic.hpp>
#include <metaprogramming/CRTP.hpp>
#include <utility/Bits.hpp>
#include <utility/Flags.hpp>
#include <utility/Position.hpp>

/// Provide the type Coords
#define PROVIDE_COORDS				\
  /*! Type to hold sizes, coordinate, etc */	\
  typedef std::array<Coord,NDims> Coords;	\
  /*! Type to hold the neighbors */		\
  typedef std::array<Coord,2*NDims> Neigh

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
  
  /// List of flags
  enum class GridFlag{HASHED,           ///< Hash data
		      SHIFTED_BC,       ///< Implements shifted bounday conditions
		      ALWAYS_WRAPPING}; ///< Force wrapping
  
  /// Default parameters for grid
  constexpr int GRID_DEFAULT_FLAGS=
    combineFlags<GridFlag::HASHED>;
  
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
		     true>
  {
    
    PROVIDE_COORDS;
    
  private:
    
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
	CRTP_THIS.volume();
      
      // Check hashability
      const auto& maxHashability=
	tb.max_size();
      if(static_cast<decltype(maxHashability)>(volume)>maxHashability)
	CRASH<<"Cannot hash a volume of "<<volume<<", max allowed: "<<(int)maxHashability;
      
      // Resize the hash table
      tb.resize(volume);
      
      // Fill the table
      CRTP_THIS.forAllPoints([&](Idx i)
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
			      CRTP_THIS.computeCoordsOfPoint(i);
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
			    CRTP_THIS.forAllOriDirs([&](int oriDir)
						    {
						      out[oriDir]=
							CRTP_THIS.computeNeighOfPoint(i,oriDir);
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
      CRTP_THIS.assertPointIsInRange(i);
      
      return
	coordsOfPointsHashTable[i];
    }
    
    /// Return the neighbor in the given oriented dir
    Idx neighOfPoint(const Idx i,
		     const int oriDir)
      const
    {
      CRTP_THIS.assertPointIsInRange(i);
      CRTP_THIS.assertOriDirIsInRange(oriDir);
      
      return
	neighsOfPointsHashTable[i][oriDir];
    }
    
    /// Tag asserting that hashing
    static constexpr char hashingTag[]=
      "Hashing";
    
  };
  
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
		     false>
  {
    
    PROVIDE_COORDS;
    
  public:
    
    PROVIDE_CRTP_CAST_OPERATOR(T);
    
    /// Get the coords of given point, computing it
    Coords coordsOfPoint(Idx i)
      const
    {
      return
	CRTP_THIS.computeCoordsOfPoint(i);
    }
    
    /// Return the neighbor in the given oriented dir, computing it
    Idx neighOfPoint(const Idx i,
		     const int oriDir)
      const
    {
      return
	CRTP_THIS.computeNeighOfPoint(i,oriDir);
    }
    
    /// Fill all the HashTables (dummy version)
    void fillHashTables()
      const
    {
    }
    
    /// Tag asserting not hashing
    static constexpr char hashingTag[]=
      "Not Hashing";
    
  };
  
  /////////////////////////////////////////////////////////////////
  
  /// Shifted boundary condition feature
  ///
  /// Forward declaration
  template <typename T,      // External type inheriting from
	    int NDims,       // Number of dimensions
	    typename Coord,  // Type of coordinate values
	    typename Idx,    // Type of index of points
	    bool Shifting>   // Flag to choose if shifting is supported or not
  class GridShiftableBC;
  
  /// Shifted boundary condition feature
  ///
  /// Version supporting shift
  template <typename T,      // External type inheriting from
	    int NDims,       // Number of dimensions
	    typename Coord,  // Type of coordinate values
	    typename Idx>    // Type of index of points
  class GridShiftableBC<T,
			NDims,
			Coord,
			Idx,
			true>
  {
    
    PROVIDE_COORDS;
    
    /// Direction identifying the face which is shifted
    int _shiftedFace{0};
    
    /// Matrix storing the shift induced on each direction
    Coords _shiftOfBC{0};
    
  public:
    
    PROVIDE_CRTP_CAST_OPERATOR(T);
    
    /// Set shift for boundary
    void setShiftBC(const int dir,       ///< Direction
		    const Coords& shift) ///< Amount to shift
    {
      // Check null shift at face dir
      if(_shiftOfBC[dir]!=0)
	CRASH<<"Shift of shiftedFace "<<dir<<"must be zero, it is"<<_shiftOfBC[dir];
      
      // Set shifted face
      _shiftedFace=
	dir;
      
      // Set shift at BC
      CRTP_THIS.forAllDims([&](int mu)
			   {
			     _shiftOfBC[mu]=
			       safeModulo(shift[mu],CRTP_THIS.side(mu));
			   });
      
      /// Trigger hashtables rebuild
      CRTP_THIS.fillHashTables();
    }
    
    /// Gets the shifting face
    int shiftedFace() const
    {
      return _shiftedFace;
    }
    
    /// Gets the shifting of the given direction
    Coord shiftOfBC(int mu) const
    {
      return _shiftOfBC[mu];
    }
    
    /// Returns the shifted coord due to boundary passing
    Coord getShiftedCoordPerpToMove(const Coords& in,      ///< Input coords
				    const int ori,         ///< Orientation
				    const int moveDir,     ///< Face of move
				    const int perpDir,     ///< Direction perpendicular to move
				    const Coord nBCpassed) ///< Number of BC passed (positive number)
      const
    {
      if(shiftedFace()==moveDir)
	{
	  /// Offset for the shift due to BC
	  const Coord muOffset=
	    this->shiftOfBC(perpDir)*moveOffset[ori]*nBCpassed;
	  
	  /// Raw destination
	  const Coord rawMuDest=
	    in[perpDir]+muOffset;
	  
	  /// Returned value
	  const Coord out=
	    safeModulo(rawMuDest,CRTP_THIS.side(perpDir));
	  
	  return out;
	}
      else
	return
	  in[perpDir];
    }
  };
  
  /// Shifted boundary condition feature
  ///
  /// Version not supporting shift
  template <typename T,      // External type inheriting from
	    int NDims,       // Number of dimensions
	    typename Coord,  // Type of coordinate values
	    typename Idx>    // Type of index of points
  class GridShiftableBC<T,
			NDims,
			Coord,
			Idx,
			false>
  {
    PROVIDE_COORDS;
    
  public:
    
    /// Gets the shifting face
    constexpr int shiftedFace()
      const
    {
      return NOT_PRESENT;
    }
    
    /// Gets the shifting of the given direction
    constexpr int shiftOfBC(int mu)
      const
    {
      return
	0;
    }
    
    /// Returns the shifted coord due to boundary passing
    ///
    /// Dummy version
    Coord getShiftedCoordPerpToMove(const Coords& in,      ///< Input coords
				    const int ori,         ///< Orientation
				    const int moveDir,     ///< Face of move
				    const int perpDir,     ///< Direction perpendicular to move
				    const Coord nBCpassed) ///< Number of BC passed (positive number)
      const
    {
      return in[perpDir];
    }
  };
  
  /////////////////////////////////////////////////////////////////
  
  /// A grid of points spanning an hypercubic grid
  template <int NDims=4,                                   // Number of dimensions
	    typename Coord=int32_t,                        // Type of coordinate values
	    typename Idx=int64_t,                          // Type of index of points
	    int Flags=GRID_DEFAULT_FLAGS>                  // Flags
  class Grid :
    public GridHashable<Grid<NDims,Coord,Idx,Flags>,
			NDims,
			Coord,
			Idx,
			getFlag<Flags,GridFlag::HASHED>>,
    public GridShiftableBC<Grid<NDims,Coord,Idx,Flags>,
			   NDims,
			   Coord,
			   Idx,
			   getFlag<Flags,GridFlag::SHIFTED_BC>>
  {
  public:
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
      _volume=
	1;
      
      // Loop on all dimension, taking product
      forAllDims([&](int mu)
		{
		  _volume*=
		    side(mu);
		});
    }
    
  public:
    
    /// Flags of the class
    static constexpr int flags=
      Flags;
    
    /// Extract the flag determining whether is hashing
    static constexpr bool isHashing=
      getFlag<flags,GridFlag::HASHED>;
    
    /// Extract the flag determining whether BC might be shifted
    static constexpr bool isShiftingBC=
      getFlag<flags,GridFlag::SHIFTED_BC>;
    
    /// Number of dimensions
    static constexpr int nDims=
      NDims;
    
    /// Number of oriented directions
    static constexpr int nOriDirs=
      2*NDims;
    
    /// Get the volume
    const Idx& volume() const
    {
      return
	_volume;
    }
    
    /// Get the given side
    Coord side(const int mu) const
    {
      return
	_sides[mu];
    }
    
    /// Check that a given point is in range
    void assertPointIsInRange(const Idx& i) const
    {
      if constexpr(GRID_DEBUG)
	if(i<0 or i>=volume())
	  CRASH<<"Cannot access to element "<<i;
    }
    
    /// Check that a given oriented dir is in range
    void assertOriDirIsInRange(const int& oriDir) const
    {
      if constexpr(GRID_DEBUG)
        if(oriDir<0 or oriDir>=2*NDims)
	  CRASH<<"Cannot use oriented dir "<<oriDir<<"out of range [0,"<<2*NDims<<"]";
    }
    
    /// Check that a given set of coords are in range
    void assertCoordsAreInRange(const Coords& cs) const
    {
      if constexpr(GRID_DEBUG)
        forAllDims([&](int mu)
		  {
		    /// Maximal value
		    const Coord& m=
		      side(mu);
		    
		    /// Coord mu value
		    const Coord& c=
		      cs[mu];
		    
		    if(c<0 or c>=m)
		      CRASH<<"Cannot have dimension "<<mu<<" equal to "<<c<<", negative or larger than "<<m;
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
      const
    {
      
      for(Idx i=0;i<volume();i++)
	f(i);
    }
    
    /// Orientation of an oriented directions
    static Orientation oriOfOriDir(const int oriDir)
    {
      /// Output orientation
      Orientation out;
      
      // Check smallest bit
      if(bitOf(oriDir,0))
	out=
	  FW;
      else
	out=
	  BW;
      
      return
	out;
    }
    
    /// Dimension of an oriented directions
    int dimOfOriDir(const int oriDir) const
    {
      // Divide by two
      return
	oriDir>>1;
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
      
      if constexpr(isHashing)
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
	  
	  /// Quozient, corresponding to the index of the remaining \c nDims-1 components
	  const Idx q=i/d;
	  
	  /// Remainder, corresponding to the coordinate
	  const Coord r=i-d*q;
	  
	  // Store the component
	  c[mu]=r;
	  
	  // Iterate on the remaining nDims-1 components
	  i=q;
	}
      
      return
	c;
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
		    side(mu);
		  
		  // Increment the coordinate
		  out=
		    out*s+cs[mu];
		});
      
      return
	out;
    }
    
    /// Returns the coordinates shifted in the asked direction
    ///
    /// Periodic boundary conditions are always assumed
    Coords shiftedCoords(const Coords& in,     ///< Input coordinates
			 const int oriDir,     ///< Oriented direction
			 const Coord amount=1) ///< Number of steps
      const
    {
      /// Returned coordinates
      Coords out;
      
      /// Orientation
      const int ori=
    	oriOfOriDir(oriDir);
      
      /// Direction to shift
      const int moveDir=
    	dimOfOriDir(oriDir);
      
      /// Offset to add
      const Coord offset=
    	moveOffset[ori]*amount;
      
      /// Destintion not considering wrap
      const Coord rawDest=
	in[moveDir]+offset;
      
      /// Actual destintion
      const Coord dest=
	safeModulo(rawDest,side(moveDir));
      
      /// Absolute number of boundaries passed
      const Coord nBCpassed=
	((rawDest<0)?(-rawDest+side(moveDir)):rawDest)/side(moveDir);
      
      forAllDims([&](int mu)
    		 {
    		   if(mu!=moveDir)
		     out[mu]=
		       this->getShiftedCoordPerpToMove(in,ori,moveDir,mu,nBCpassed);
		   else
    		     out[mu]=
		       dest;
    		 });
      
      return
	out;
    }
    
    /// Compute the neighbor in the oriented direction oriDir of point i
    Idx computeNeighOfPoint(const Idx i,        ///< Point
			    const int oriDir)   ///< Oriented direction
      const
    {
      assertPointIsInRange(i);
      assertOriDirIsInRange(oriDir);
      
      return
	pointOfCoords(shiftedCoords(this->coordsOfPoint(i),oriDir));
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
