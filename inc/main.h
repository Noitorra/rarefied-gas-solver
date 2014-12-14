#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <memory>
#include <string>

#include "types.h"

namespace sep {
  enum GridGeometry {
    COMB_GRID_GEOMETRY = 0,
    H_GRID_GEOMTRY
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

  enum MacroParamType {
    T_PARAM = 0, // Temperature
    C_PARAM, // Concentration
    P_PARAM, // Pressure
    FLOW_PARAM // Flow
  };

  enum NeighborType {
    PREV = 0,
    NEXT = 1
  };

  enum BoundaryType
  {
    BT_DIFFUSE,
    BT_STREAM,
    BT_PRESSURE
  };
}

class MacroData {
public:
  MacroData() :
          C(1.0),
          T(1.0),
          P(1.0)
  {};

  double C; // concentration
  double T; // temperature
  double P; // pressure

  Vector3d Stream;
  Vector3d HeatStream;
};

class CellConfig {
public:
  CellConfig() :
  C(1.0),
  T(1.0),
  wall_T(1.0),
  boundary_cond(sep::BT_DIFFUSE)
  {};

  double C; // concentration
  double T; // temperature
  double wall_T;  // wall temperature

  sep::BoundaryType boundary_cond;
};