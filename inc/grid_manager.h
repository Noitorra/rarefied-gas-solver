#pragma once
#include "main.h"
#include "types.h"

class Grid;
class Config;
class Cell;

class InitCellData {
  friend class GridManager;
protected:
  InitCellData(sep::CellType eType) :
  m_eType(eType) {};
  
  sep::CellType m_eType;
  Cell* m_pCell;
};

class GridManager {
public:
  GridManager();
  void SetParent(Grid* pGrid);
  void SaveGridConfig(Config* pConfig);
  void LoadGridConfig(Config* pConfig);
  
private:
  bool Write(const std::string& sName);
  bool Read(const std::string& sName);
  // Throws char const*
  const std::string GenerateFileName(sep::GridGeometry eGeometry) const;
  void Build(Config* pConfig);
  void BuildCombTypeGrid(Config* pConfig);
  void BuildHTypeGrid(Config* pConfig);
  void InitEmptyBox(const Vector3i& vSize);
  void SetBox(const Vector3i& vStart, const Vector3i& vSize, sep::CellType eType);
  void FillInGrid(Config* pConfig);


  class Grid* m_pGrid;
  std::vector<std::vector<std::vector<std::shared_ptr<InitCellData>>>> m_vCells;
};