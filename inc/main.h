#ifndef MAIN_H_
#define MAIN_H_

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
}

class MacroData {
public:
  MacroData() :
  Concentration(1.0),
  Temperature(1.0)
  {};
	// Concentration
	double Concentration;
	double Temperature;
  
	Vector3d Stream;
	Vector3d HeatStream;
};


#endif // MAIN_H_