#pragma once
#include "main.h"

class Config {
public:
  Config(const std::string& sName = "default") :
  m_sName(sName) {
    // Default configs
    m_vGridSize = std::vector<int>(3);
    m_vGridSize[sep::X] = m_vGridSize[sep::Y] = 10;
    m_vGridSize[sep::Z] = 1;
    
    m_eGridGeometryType = sep::DIMAN_GRID_GEOMETRY;
  };
  ~Config() {};
  
  const std::string& GetName() const { return m_sName; }
  void SetGridSize(const std::vector<int>& vGridSize);
  void PrintMe();
  void SetGridGeometryType(sep::GridGeometry eGridGeometryType);
  sep::GridGeometry GetGridGeometryType() const { return m_eGridGeometryType; };
private:
  // Please, add new members to PrintMe() function
  std::string m_sName;
  std::vector<int> m_vGridSize;
  sep::GridGeometry m_eGridGeometryType;
};