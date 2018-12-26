#ifndef _GRID_HPP
#define _GRID_HPP

/// \file Grid.hpp
///
/// \brief Defines hypercubic grids

#include <array>

#include <metaprogramming/CRTP.hpp>

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
    
    /// Hashed volume
    Idx volume;
    
  public:
    
    /// Set the volume, computing it with external routine
    void setVolume()
    {
      volume=(~*this).computeVolume();
    }
    
    /// Get the volume
    Idx getVolume() const
    {
      return volume;
    }
    
    /// Flag asserting that hashing
    static constexpr char isHashing[]=
      "Hashing";
    
  };
  
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
    
  public:
    
    /// Set the volume, doing nothing
    void setVolume()
    {
    }
    
    /// Get the volume
    Idx getVolume() const
    {
      return (~*this).computeVolume();
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
  public:
    
    /// Type to hold sizes, coordinate, etc
    typedef std::array<Coord,NDim> Coords;
    
  private:
    
    /// Side of the grid
    Coords sides;
    
  public:
    
    /// Number of dimensions
    static constexpr int nDim=
      NDim;
    
    /// Compute the volume
    Idx computeVolume() const
    {
      Idx volume=1;
      for(auto &c : sides)
	volume*=c;
      
      return volume;
    }
    
    /// Set the sides and trigger the volume change
    void setSides(const Coords& extSides)
    {
      sides=extSides;
      
      this->setVolume();
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

#endif
