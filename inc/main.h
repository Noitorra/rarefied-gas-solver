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
  
  enum CellType {
    EMPTY_CELL = 0,
    NORMAL_CELL,
    FAKE_CELL
  };
  
  enum Axis {
    X = 0,
    Y,
    Z
  };
}
