#ifndef GRID_H_
#define GRID_H_

#include "main.h"
#include <memory>
#include "types.h"

class Cell;
class GridManager;
class Solver;
class OutResults;
struct Config;
class VesselGrid;
class Grid;

class InitCellData {
  friend GridManager;
  friend Grid;
  friend OutResults;
protected:
  InitCellData(sep::CellType eType) :
  m_eType(eType),
  m_pCell(nullptr) {
    m_vMacroData.resize(2);
  };
  
  sep::CellType m_eType;
  Cell* m_pCell;
  std::vector<MacroData> m_vMacroData;
  MacroData m_cInitCond;
};

// This is only this process part of whole grid
class Grid {
  friend GridManager;
public:
  Grid();
  void Init(GridManager* pGridManager);

  const Vector3i& GetSize() const { return m_vSize; }

  std::vector<std::shared_ptr<Cell>>& GetCells();
  std::vector<std::vector<std::vector<std::shared_ptr<InitCellData>>>>& GetInitCells();
  InitCellData* GetInitCell(Vector2i vP);
  std::vector<std::shared_ptr<VesselGrid>>& GetVessels();

private:
  void AddCell(std::shared_ptr<Cell> pCell);
  void AllocateInitData();
  
  Vector3i m_vSize;

  GridManager* m_pGridManager;
  
  std::vector<std::shared_ptr<Cell>> m_vCells;
  std::vector<std::vector<std::vector<std::shared_ptr<InitCellData>>>> m_vInitCells;
  std::vector<std::shared_ptr<VesselGrid>> m_vVessels;
};

#endif // GRID_H_
