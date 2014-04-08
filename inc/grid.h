#pragma once
#include "main.h"
#include <memory>
#include "types.h"

class Cell;
class GridManager;
class Solver;
class OutResults;
class Config;

// this is only this process part of whole grid
class Grid {
  friend GridManager;
public:
  Grid();
  
  const Vector3i& GetSize() const {
    return m_vSize;
  }
  const Vector3i& GetStart() const {
    return m_vStart;
  }
  const Vector3i& GetWholeSize() const {
    return m_vWholeSize;
  }

  void SetParent(class Solver* pSolver) { m_pSolver = pSolver; }
  Solver* getSolver() { return m_pSolver; }

  void Init();
  void AddCell(std::shared_ptr<Cell> pCell);
  std::vector<std::shared_ptr<Cell>>& getCellVector();

  GridManager* GetGridManager() const;
  OutResults* GetOutResults() const;
  void BuildWithActiveConfig();
  Config* GetConfig() const;

private:
  // size of this
  Vector3i m_vSize; // this process grid size
  Vector3i m_vStart; // place where this grid starts in whole grid
  Vector3i m_vWholeSize; // size of whole grid

  std::shared_ptr<GridManager> m_pGridManager;
  std::shared_ptr<OutResults> m_pOutResults;
  Solver* m_pSolver;
  
  std::vector<std::shared_ptr<Cell>> m_vCells;
};
