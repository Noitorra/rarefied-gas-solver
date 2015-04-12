#ifndef CONFIG_H_
#define CONFIG_H_

#include "main.h"

class Gas;

typedef std::vector<std::shared_ptr<Gas>> GasVector;

struct HTypeGridConfig {
  int D;
  int l;
  int d;
  int h;
  int gaps_q;
  double T1, T2;
  double n1, n2, n3, n4;
};

struct Config {
  static std::string sName;
  static Vector3i vGridSize;
  static sep::GridGeometry eGridGeometryType;
  static double dTimestep;
  static int iMaxIteration;
  static bool bUseIntegral;
  static std::shared_ptr<HTypeGridConfig> pHTypeGridConfig;
  static std::string sOutputPrefix;
  static Vector2d vCellSize;  // default cell size in mm
  static int iGasesNumber;
  static GasVector vGas;
  static bool bGPRTGrid;
  static int iOutEach;

  static double n_normalize;
  static double T_normalize;
  static double P_normalize; // real normilize operation
  static double tau_normalize;
  static double m_normalize;
  static double e_cut_normalize;
  static double l_normalize; // the mean free path of a molecule

  static void Init();
  static void PrintMe();
};

#endif // CONFIG_H_