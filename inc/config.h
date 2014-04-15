#ifndef CONFIG_H_
#define CONFIG_H_

#include "main.h"
#include "types.h"

class HTypeGridConfig {
public:
  int D;
  int l;
  int d;
  int h;
  int gaps_q;
  double T1, T2;
  double n1, n2, n3, n4;
};

class Config {
public:
  Config(const std::string& sName = "default") :
  m_sName(sName),
  m_vGridSize(Vector3i(10, 10, 1)),
  m_vOutputSize(Vector3i(10, 10, 1)),
  m_vOutputGridStart(Vector3i()),
  m_eGridGeometryType(sep::COMB_GRID_GEOMETRY),
  m_dTimestep(0.01),
  m_iMaxIteration(100),
  m_bUseIntegral(true),
  m_pHTypeGridConfig(new HTypeGridConfig),
  m_sOutputPrefix("../") {};
  
  void SetGridSize(const Vector3i& vGridSize);
  void SetOutputSize(const Vector3i& vOutputSize);
  void SetOutputGridStart(const Vector3i& vOutputGridStart);
  void SetGridGeometryType(sep::GridGeometry eGridGeometryType);
  void SetTimeStep(double dTimeStep) { m_dTimestep = dTimeStep; }
  void SetMaxIteration(int iMaxIteration) { m_iMaxIteration = iMaxIteration; }
  void SetUseIntegral(bool bUseIntegral) { m_bUseIntegral = bUseIntegral; }
  void SetOutputPrefix(std::string sOutputPrefix) { m_sOutputPrefix = sOutputPrefix; }
  
  const Vector3i& GetGridSize() const { return m_vGridSize; }
  const Vector3i& GetOutputSize() const { return m_vOutputSize; }
  const Vector3i& GetOutputGridStart() const { return m_vOutputGridStart; }
  sep::GridGeometry GetGridGeometryType() const { return m_eGridGeometryType; };
  double GetTimeStep() const { return m_dTimestep; }
  int GetMaxIteration() const { return m_iMaxIteration; }
  bool GetUseIntegral() const { return m_bUseIntegral; }
  const std::string& GetOutputPrefix() const { return m_sOutputPrefix; }
  const std::string& GetName() const { return m_sName; }
  HTypeGridConfig* GetHTypeGridConfig() { return m_pHTypeGridConfig.get(); }
  
  void PrintMe();
  
private:
  // Please, add new members to PrintMe() function
  std::string m_sName;
  Vector3i m_vGridSize;
  Vector3i m_vOutputGridStart;
  Vector3i m_vOutputSize;
  sep::GridGeometry m_eGridGeometryType;
	double m_dTimestep;
	int m_iMaxIteration;
  bool m_bUseIntegral;
  std::shared_ptr<HTypeGridConfig> m_pHTypeGridConfig;
  std::string m_sOutputPrefix;
};

#endif // CONFIG_H_