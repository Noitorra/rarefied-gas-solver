#pragma once
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
  m_eGridGeometryType(sep::DIMAN_GRID_GEOMETRY),
  m_dTimestep(0.01),
  m_iMaxIteration(100),
  m_bUseIntegral(true),
  m_bUseVessels(true),
  m_bUseLooping(false),
  m_pHTypeGridConfig(new HTypeGridConfig),
  m_iAdditionalVesselLenght(0),
  m_bUseInitialConditions(false),
  m_bResetSomeCellsEachIter(false),
  m_sOutputPrefix("../") {};
  ~Config() {};
  
  const std::string& GetName() const { return m_sName; }
  void SetGridSize(const Vector3i& vGridSize);
  const Vector3i& GetGridSize() const { return m_vGridSize; }
  void SetOutputSize(const Vector3i& vOutputSize);
  const Vector3i& GetOutputSize() const { return m_vOutputSize; }
  void SetOutputGridStart(const Vector3i& vOutputGridStart);
  const Vector3i& GetOutputGridStart() const { return m_vOutputGridStart; }
  void PrintMe();
  void SetGridGeometryType(sep::GridGeometry eGridGeometryType);
  sep::GridGeometry GetGridGeometryType() const { return m_eGridGeometryType; };
  double GetTimeStep() const { return m_dTimestep; }
  void SetTimeStep(double dTimeStep) { m_dTimestep = dTimeStep; }
  int GetMaxIteration() const { return m_iMaxIteration; }
  void SetMaxIteration(int iMaxIteration) { m_iMaxIteration = iMaxIteration; }
  bool GetUseIntegral() const { return m_bUseIntegral; }
  void SetUseIntegral(bool bUseIntegral) { m_bUseIntegral = bUseIntegral; }
  bool GetUseVessels() const { return m_bUseVessels; }
  void SetUseVessels(bool bUseVessels) { m_bUseVessels = bUseVessels; }
  bool GetUseLooping() const { return m_bUseLooping; }
  void SetUseLooping(bool bUseLooping) { m_bUseLooping = bUseLooping; }
  HTypeGridConfig* GetHTypeGridConfig() {
    return m_pHTypeGridConfig.get();
  }
  void SetAdditionalVesselLenght(int iAdditionalVesselLenght) {
    m_iAdditionalVesselLenght = iAdditionalVesselLenght;
  }
  int GetAdditionalVesselLenght() const {
    return m_iAdditionalVesselLenght;
  }
  void SetUseInitialConditions(bool bUseInitialConditions) { m_bUseInitialConditions = bUseInitialConditions; }
  bool GetUseInitialConditions() const { return m_bUseInitialConditions; }
  void SetResetSomeCellsEachIter(bool bResetSomeCellsEachIter) { m_bResetSomeCellsEachIter = bResetSomeCellsEachIter; }
  bool GetResetSomeCellsEachIter() const { return m_bResetSomeCellsEachIter; }
  void SetOutputPrefix(std::string sOutputPrefix) { m_sOutputPrefix = sOutputPrefix; }
  const std::string& GetOutputPrefix() const { return m_sOutputPrefix; }
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
  bool m_bUseVessels;
  bool m_bUseLooping;
  std::shared_ptr<HTypeGridConfig> m_pHTypeGridConfig;
  int m_iAdditionalVesselLenght;
  bool m_bUseInitialConditions;
  bool m_bResetSomeCellsEachIter;
  std::string m_sOutputPrefix;
};