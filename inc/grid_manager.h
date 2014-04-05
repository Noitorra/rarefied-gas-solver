#pragma once
#include "main.h"

class Grid;
class Config;

class GridManager {
public:
  GridManager();
  void SetParent(Grid* pGrid);
  void SaveGridConfig(Config* pConfig);
  void LoadGridConfig(Config* pConfig);
  
private:
  bool Write(const std::string& sName);
  bool Read(const std::string& sName);
  void Build(Config* pConfig);
  // Throws char const*
  const std::string GenerateFileName(sep::GridGeometry eGeometry) const;

  class Grid* m_pGrid;
};