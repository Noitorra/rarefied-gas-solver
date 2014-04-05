#pragma once
#include "main.h"

class Cell;
class GridManager;

// this is only this process part of whole grid
class Grid {
public:
  Grid();
  
  const std::vector<int>& GetSize() const {
    return m_vSize;
  }
  const std::vector<int>& GetStart() const {
    return m_vStart;
  }
  const std::vector<int>& GetWholeSize() const {
    return m_vWholeSize;
  }

  void SetParent(class Solver* pSolver) { m_pSolver = pSolver; }
  void Init();
  void AddCell(std::shared_ptr<Cell> pCell);
  GridManager* GetGridManager() const;
  void BuildWithActiveConfig();

private:
  // size of this
  std::vector<int> m_vSize; // this process grid size
  std::vector<int> m_vStart; // place where this grid starts in whole grid
  std::vector<int> m_vWholeSize; // size of whole grid

  std::shared_ptr<class GridManager> m_pGridManager;
  class Solver* m_pSolver;
  
  std::vector<std::shared_ptr<Cell>> m_vCells;
};