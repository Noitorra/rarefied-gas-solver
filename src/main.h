#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <memory>
#include <string>
#include <functional>

#include "utilities/types.h"

#define NOMINMAX
#include <tbb/tbb.h>
#include <sstream>

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
    Y = 1,
    Z = 2
  };

  enum MacroParamType {
    T_PARAM = 0, // Temperature
    C_PARAM, // Concentration
    P_PARAM, // Pressure
    FLOW_PARAM, // Flow
    LAST_PARAM
  };

  enum NeighborType {
    PREV = 0,
    NEXT = 1
  };

  enum BoundaryType {
    BT_DIFFUSE,
    BT_GASE,
    BT_MIRROR,
    BT_FLOW
  };

  const double k = 1.38e-23;  // Bolzman const
}

class MacroData {
public:
  MacroData() :
    C(1.0),
    T(1.0),
    P(1.0) {
  };

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
    boundary_pressure(1.0),
    locked_axes(-1) {
  };

  double pressure; // initial pressure
  double T; // initial temperature
  sep::BoundaryType boundary_cond;
  double boundary_T;  // wall temperature
  double boundary_pressure;
  Vector3d boundary_stream;
  int locked_axes;
};

typedef std::map<int, CellConfig> GasesConfigsMap;

struct GridBox {
  Vector2i p;     // in cells
  Vector2i size;  // in cells
  CellConfig def_config;

  virtual void config(int x, int y, GasesConfigsMap& configs, const Vector2i& size, const Vector2i& start) = 0;
};

template <typename T>
inline int sgn(T val) {
  return (T(0) < val) - (val < T(0));
}

template <typename T>
std::string to_string(T const & value) {
  std::stringstream ss;
  ss << value;
  return ss.str();
}