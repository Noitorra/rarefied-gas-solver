#pragma once
#include "main.h"
#include "types.h"

class Config {
public:
  Config(const std::string& sName = "default") :
  m_sName(sName),
  m_vGridSize(Vector3i(10, 10, 1)),
  m_eGridGeometryType(sep::DIMAN_GRID_GEOMETRY),
  m_dTimestep(0.01),
  m_iMaxIteration(100) {};
  ~Config() {};
  
  const std::string& GetName() const { return m_sName; }
  void SetGridSize(const Vector3i& vGridSize);
  const Vector3i& GetGridSize() const { return m_vGridSize; }
  void PrintMe();
  void SetGridGeometryType(sep::GridGeometry eGridGeometryType);
  sep::GridGeometry GetGridGeometryType() const { return m_eGridGeometryType; };
  double GetTimeStep() const { return m_dTimestep; }
  void SetTimeStep(double dTimeStep) { m_dTimestep = dTimeStep; }
  int GetMaxIteration() const { return m_iMaxIteration; }
  void SetMaxIteration(int iMaxIteration) { m_iMaxIteration = iMaxIteration; }
private:
  // Please, add new members to PrintMe() function
  std::string m_sName;
  Vector3i m_vGridSize;
  sep::GridGeometry m_eGridGeometryType;
	double m_dTimestep;
	int m_iMaxIteration;
};