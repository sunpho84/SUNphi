#ifndef _GRID_HPP
#define _GRID_HPP

/// \file Grid.hpp
///
/// \brief Defines hypercubic grids

#include <array>

namespace SUNphi
{
  /// A grid of points spanning an hypercubic grid
  template <int NDim=4,              // Number of dimensions
	    typename Coord=int32_t,  // Type of coordinate values
	    typename Idx=int64_t>    // Type of index of points
  class Grid
  {
  public:
    
    /// Number of dimensions
    static constexpr int nDim=
      NDim;
    
    /// Type to hold sizes, coordinate, etc
    typedef std::array<Coord,NDim> Coords;
    
    /// Side of the grid
    const Coords sides;
    
    /// Total volume
    Idx volume;
    
    /// Construct from sides
    Grid(const Coords& sides) :
      sides(sides)
    {
      volume=1;
      for(auto &c : sides)
	volume*=c;
    }
  };
  
  /// Deduction guide for bracket list
  template <int NDim,
	    typename Coord>
  Grid(const Coord(&sides)[NDim])
    -> Grid<NDim,int>;
}

#endif
