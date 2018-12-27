#ifndef _GRID_HPP
#define _GRID_HPP

/// \file Grid.hpp
///
/// \brief Defines hypercubic grids

#include <array>
#include <vector>

#include <metaprogramming/CRTP.hpp>
#include <system/Debug.hpp>

/// Provide the type Coords
#define PROVIDE_COORDS				\
  /*! Type to hold sizes, coordinate, etc */	\
    typedef std::array<Coord,NDim> Coords

/// Flag to enable Grid debug
#define GRID_DEBUG \
  true

namespace SUNphi
{
  /// Constant asserting to hash
  static constexpr bool HASHING=
    true;
  
  /// Constant asserting not to hash
  static constexpr bool NOT_HASHING=
    false;
  
  /////////////////////////////////////////////////////////////////
  
  /// Hashable properties of a \c Grid
  ///
  /// Forward implementation
  template <typename T,      // External type inheriting from
	    int NDim,        // Number of dimensions
	    typename Coord,  // Type of coordinate values
	    typename Idx,    // Type of index of points
	    bool Hashing>    // Store or not tables
  class GridHashable;
  
  /// Hashable properties of a \c Grid
  ///
  /// Hashing version
  template <typename T,      // External type inheriting from
	    int NDim,        // Number of dimensions
	    typename Coord,  // Type of coordinate values
	    typename Idx>    // Type of index of points
  class GridHashable<T,
		     NDim,
		     Coord,
		     Idx,
		     HASHING>
  {
    
    PROVIDE_CRTP_CAST_OPERATOR(T);
    
    PROVIDE_COORDS;
    
    /// Hashed coords of all points
    std::vector<Coords> coordsOfPointsHashTable;
    
    /// Set the hash table of coordinates of all points
    void fillCoordsOfPointsHashTables()
    {
      /// Proxy volume
      const Idx volume=
	CRTP_CAST.volume();
      
      // Check hashability
      if(volume>coordsOfPointsHashTable.max_size())
	CRASH("Cannot hash a volume of",volume);
      
      // Resize the hash table of coordsOfPoints
      coordsOfPointsHashTable.resize(volume);
      
      // Fill the table
      CRTP_CAST.forAllPoints([&](Idx i)
			     {
			       coordsOfPointsHashTable[i]=
				 CRTP_CAST.computeCoordsOfPoint(i);
			     });
    }
    
  public:
    
    /// Fill all the HashTables
    void fillHashTables()
    {
      fillCoordsOfPointsHashTables();
    }
    
    /////////////////////////////////////////////////////////////////
    
    /// Get the coords of given point, computing it
    Coords coordsOfPoint(Idx i) const
    {
      CRTP_CAST.assertPointIsInRange(i);
      
      return coordsOfPointsHashTable[i];
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
	    int NDim,        // Number of dimensions
	    typename Coord,  // Type of coordinate values
	    typename Idx>    // Type of index of points
  class GridHashable<T,
		     NDim,
		     Coord,
		     Idx,
		     NOT_HASHING>
  {
    
    PROVIDE_CRTP_CAST_OPERATOR(T);
    
    PROVIDE_COORDS;
    
  public:
    
    /// Get the coords of given point, computing it
    Coords getCoordsOfPoint(Idx i) const
    {
      return CRTP_CAST.computeCoordsOfPoint(i);
    }
    
    /// Flag asserting not hashing
    static constexpr char isHashing[]=
      "Not Hashing";
    
  };
  
  /////////////////////////////////////////////////////////////////
  
  /// A grid of points spanning an hypercubic grid
  template <int NDim=4,              // Number of dimensions
	    typename Coord=int32_t,  // Type of coordinate values
	    typename Idx=int64_t,    // Type of index of points
	    bool Hashing=true>       // Store or not tables
  class Grid :
    public GridHashable<Grid<NDim,Coord,Idx,Hashing>,
			NDim,
			Coord,
			Idx,
			Hashing>
  {
    PROVIDE_COORDS;
    
  private:
    
    /// Side of the grid
    Coords sides;
    
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
		  _volume*=sides[mu];
		});
    }
    
  public:
    
    /// Number of dimensions
    static constexpr int nDim=
      NDim;
    
    /// Get the volume
    const Idx& volume() const
    {
      return _volume;
    }
    
    /// Check that a given point is in range
    void assertPointIsInRange(const Idx& i) const
    {
      if constexpr(GRID_DEBUG)
	if(i<0 or i>=volume())
	  CRASH("Cannot access to element",i);
    }
    
    /// Check that a given set of coords are in range
    void assertCoordsAreInRange(const Coords& cs) const
    {
      if constexpr(GRID_DEBUG)
        forAllDims([&](int mu)
		  {
		    /// Maximal value
		    const Coord& m=
		      CRTP_CAST.sides[mu];
		    
		    /// Coord mu value
		    const Coord& c=
		      cs[mu];
		    
		    if(c<0 or c>=m)
		      CRASH("Cannot have dimension",mu,"equal to",c,", negative or larger than",m);
		  });
    }
    
    /// Loop on all dimension calling the passed function
    template <typename F>            // Type of the function
    void forAllDims(F&& f) const   ///< Function to be called
    {
      for(int mu=0;mu<nDim;mu++)
	f(mu);
    }
    
    /// Loop on all points calling the passed function
    template <typename F>              // Type of the function
    void forAllPoints(F&& f) const   ///< Function to be called
    {
      for(Idx i=0;i<volume();i++)
	f(i);
    }
    
    /// Set the sides and trigger the volume change
    void setSides(const Coords& extSides)
    {
      sides=extSides;
      
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
      
      for(int mu=nDim-1;mu>=0;mu--)
	{
	  /// Dividend, corresponding to the \c mu side length
	  const Coord& d=sides[mu];
	  
	  /// Quozient, corresponding to the index of the remaining NDIM-1 components
	  const Idx q=i/d;
	  
	  /// Remainder, corresponding to the coordinate
	  const Coord r=i-d*q;
	  
	  // Store the component
	  c[mu]=r;
	  
	  // Iterate on the remaining NDIM-1 components
	  i=q;
	}
      
      return c;
    }
    
    /// Compute the point of given coords
    Idx pointOfCoords(const Coords& cs)
    {
      assertCoordsAreInRange(cs);
      
      /// Returned point
      Idx out=
	0;
      
      forAllDims([&](int mu)
		{
		  /// Grid side
		  const Coord& s=
		    CRTP_CAST.sides[mu];
		  
		  // Increment the coordinate
		  out=
		    out*s[mu]+cs[mu];
		});
      
      return out;
    }
    
    /// Construct from sides
    Grid(const Coords& sides)
    {
      setSides(sides);
    }
    
  };
  
  /// Deduction guide for bracket list
  template <int NDim,
	    typename Coord>
  Grid(const Coord(&sides)[NDim])
    -> Grid<NDim,int>;
}

#undef PROVIDE_COORDS

#endif
