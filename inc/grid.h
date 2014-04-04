#pragma once
#include "main.h"

// this is only this process part of whole grid
class Grid {
public:
  Grid();
  
  const std::vector<int>& GetSize() {
    return m_vSize;
  }
  const std::vector<int>& GetStart() {
    return m_vStart;
  }
  const std::vector<int>& GetWholeSize() {
    return m_vWholeSize;
  }
  void SetParent(class Solver* pSolver) { m_pSolver = pSolver; }
  void Init();

private:
  // size of this
  std::vector<int> m_vSize; // this process grid size
  std::vector<int> m_vStart; // place where this grid starts in whole grid
  std::vector<int> m_vWholeSize; // size of whole grid
  class GridManager* m_pGridManager;
  class Solver* m_pSolver;
};