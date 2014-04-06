#pragma once
#include "main.h"
#include "types.h"

class Config {
public:
  Config(const std::string& sName = "default") :
  m_sName(sName),
  m_vGridSize(Vector3i(10, 10, 1)),
  m_eGridGeometryType(sep::DIMAN_GRID_GEOMETRY) {};
  ~Config() {};
  
  const std::string& GetName() const { return m_sName; }
  void SetGridSize(const Vector3i& vGridSize);
  const Vector3i& GetGridSize() const { return m_vGridSize; }
  void PrintMe();
  void SetGridGeometryType(sep::GridGeometry eGridGeometryType);
  sep::GridGeometry GetGridGeometryType() const { return m_eGridGeometryType; };
private:
  // Please, add new members to PrintMe() function
  std::string m_sName;
  Vector3i m_vGridSize;
  sep::GridGeometry m_eGridGeometryType;
};