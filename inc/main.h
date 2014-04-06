#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <memory>

namespace sep {
  enum GridGeometry {
    DIMAN_GRID_GEOMETRY = 0,
    PROHOR_GRID_GEOMTRY
  };
  
  // Temporary
  // Using in config.cpp because of
  // there is no Vec3.x()
  // For now using std::vector<int>[sep::X] instead!
  enum Axis {
    X = 0,
    Y,
    Z
  };
}
