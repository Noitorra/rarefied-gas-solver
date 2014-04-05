#pragma once
#include "main.h"

class GridManager {
public:
  GridManager();
  void SetParent(class Grid* pGrid);
  void SaveConfiguration(sep::Configuration config);
  void LoadConfiguration(sep::Configuration config);
  
private:
  bool Write(const std::string& name);
  bool Read(const std::string& name);
  void Build(sep::Configuration config);
  const std::string GenerateFileName(sep::Configuration config) const;

  class Grid* m_pGrid;
};