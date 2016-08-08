#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <functional>

#include "utilities/types.h"

#define NOMINMAX
#include <tbb/tbb.h>
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

class CellConfig {
public:
	CellConfig() :
		dPressure(1.0),
		dTemperature(1.0),
		eBoundaryType(sep::BT_DIFFUSE),
		dBoundaryTemperature(1.0),
		dBoundaryPressure(1.0),
		iLockedAxes(-1) {};

	double dPressure; // initial dPressure
	double dTemperature; // initial temperature
	sep::BoundaryType eBoundaryType;
	double dBoundaryTemperature; // wall temperature
	double dBoundaryPressure;
	Vector3d vBoundaryFlow;
	int iLockedAxes;
};

typedef std::map<int, CellConfig> GasesConfigsMap;

struct GridBox {
	Vector2i p; // in cells
	Vector2i size; // in cells
	CellConfig def_config;

	virtual void config(int x, int y, GasesConfigsMap& configs, const Vector2i& size, const Vector2i& start) = 0;
};

template <typename T>
inline int sgn(T val) {
	return (T(0) < val) - (val < T(0));
}

template <typename T>
std::string to_string(T const& value) {
	std::stringstream ss;
	ss << value;
	return ss.str();
}
