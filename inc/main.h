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
  
  enum MacroParamType {
    T_PARAM = 0, // Temperature
    C_PARAM, // Concentration
    FLOW_PARAM // Flow
  };
}

class MacroData {
public:
  MacroData() :
  Concentration(0.0),
  Temperature(0.0)
  {};
	// Concentration
	double Concentration;
	double Temperature;
  
	Vector3d Stream;
	Vector3d HeatStream;
};


