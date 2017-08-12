#ifndef MAIN_H_
#define MAIN_H_

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <functional>

#include "utilities/types.h"

#define NOMINMAX

#include <sstream>

namespace sep {
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



    const double BOLTZMANN_CONSTANT = 1.38e-23; // Boltzmann const // TODO: Make more precise
}

class MacroData {
public:
    MacroData() :
            dDensity(1.0),
            dTemperature(1.0),
            dPressure(1.0) {};

    double dDensity; // concentration
    double dTemperature; // temperature
    double dPressure; // dPressure

    Vector3d Stream;
    Vector3d HeatStream;
};

#endif /* MAIN_H_ */