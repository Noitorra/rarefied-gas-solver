#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <memory>
#include <string>

#include "types.h"
#include <functional>

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
//    FLOW_PARAM, // Flow
    LAST_PARAM
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
  pressure(1.0),
  T(1.0),
  boundary_cond(sep::BT_DIFFUSE),
  boundary_T(1.0),
  boundary_pressure(1.0)
  {};

  double pressure; // initial pressure
  double T; // initial temperature
  sep::BoundaryType boundary_cond;
  double boundary_T;  // wall temperature
  Vector3d boundary_stream;
  double boundary_pressure;
};

typedef std::map<int, CellConfig> GasesConfigsMap;
typedef std::function<void(int x, int y, GasesConfigsMap& configs, const Vector2i& size)> ConfigFunction;

struct GridBox {
  Vector2i p;     // in cells
  Vector2i size;  // in cells
  CellConfig def_config;

  ConfigFunction config_func;
};