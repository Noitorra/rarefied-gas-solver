#pragma once
#include "main.h"
#include "types.h"

class Grid;
class Config;
class Cell;
class OutResults;
class MacroParam;

class InitCellData {
  friend class GridManager;
  friend OutResults;
protected:
  InitCellData(sep::CellType eType) :
  m_eType(eType),
  m_pCell(nullptr),
  m_pMacroParam(nullptr) {};
  
  sep::CellType m_eType;
  Cell* m_pCell;
  std::shared_ptr<MacroParam> m_pMacroParam;
};

class GridManager {
  friend OutResults;
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
  void LinkCells(Config* pConfig);
  void AddGasBox(Vector3i vStart, Vector3i vSize, Vector3b vWithoutFakes, bool bFlatZ);
  Cell* GetNeighb(Vector3i vCoord, sep::Axis eAxis, int iSlash) ;


  Grid* m_pGrid;
  std::vector<std::vector<std::vector<std::shared_ptr<InitCellData>>>> m_vCells;
};