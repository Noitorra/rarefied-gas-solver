#ifndef GRID_H_
#define GRID_H_

#include "main.h"

class Cell;

class InitCellData {
  friend class GridManager;
  friend class Grid;
  friend class OutResults;
protected:
  InitCellData(sep::CellType eType) :
  m_eType(eType),
  m_pCell(nullptr) {
    m_vMacroData.resize(2);
  };
  
  sep::CellType m_eType;
  Cell* m_pCell;
  std::vector<MacroData> m_vMacroData;
  GasesConfigsMap m_mInitConds;  // initial configs for several gases
};

class Grid {
  friend class GridManager;
public:
  Grid();
  void Init(class GridManager* pGridManager);

  const Vector3i& GetSize() const { return m_vSize; }

  std::vector<std::shared_ptr<Cell>>& GetCells();
  std::vector<std::vector<std::vector<std::shared_ptr<InitCellData>>>>& GetInitCells();
  InitCellData* GetInitCell(Vector2i vP);

private:
  void AddCell(std::shared_ptr<Cell> pCell);
  void AllocateInitData();
  
  Vector3i m_vSize;

  GridManager* m_pGridManager;
  
  std::vector<std::shared_ptr<Cell>> m_vCells;
  std::vector<std::vector<std::vector<std::shared_ptr<InitCellData>>>> m_vInitCells;
};

#endif // GRID_H_
