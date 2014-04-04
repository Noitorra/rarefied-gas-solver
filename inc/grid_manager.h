#pragma once
#include "main.h"

class GridManager {
public:
  GridManager();
  void SetParent(class Grid* pGrid);
  bool Write(const std::string& name);
  bool Read(const std::string& name);
  
private:
  class Grid* m_pGrid;
};