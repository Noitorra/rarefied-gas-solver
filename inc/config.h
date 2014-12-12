#ifndef CONFIG_H_
#define CONFIG_H_

#include "main.h"
#include "types.h"

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
  static Vector3i vOutputGridStart;
  static Vector3i vOutputSize;
  static sep::GridGeometry eGridGeometryType;
  static double dTimestep;
  static int iMaxIteration;
  static bool bUseIntegral;
  static std::shared_ptr<HTypeGridConfig> pHTypeGridConfig;
  static std::string sOutputPrefix;

  static void PrintMe();
};

#endif // CONFIG_H_