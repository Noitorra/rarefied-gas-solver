#pragma once
#include "main.h"
#include "types.h"

class Grid;
class Config;
class Cell;
class OutResults;
class MacroData;

class InitCellData {
  friend class GridManager;
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

class GridManager {
  friend OutResults;
public:
  GridManager();
  void SetParent(Grid* pGrid);
  void SaveGridConfig(Config* pConfig);
  void LoadGridConfig(Config* pConfig);
  void Print(sep::Axis axis);
  
private:
  bool Write(const std::string& sName);
  bool Read(const std::string& sName);
  // Throws char const*
  const std::string GenerateFileName(sep::GridGeometry eGeometry) const;
  void Build(Config* pConfig);
  void BuildCombTypeGrid(Config* pConfig);
  void BuildHTypeGrid(Config* pConfig);
  void InitEmptyBox(const Vector3i& vSize);
  void SetBox(const Vector3i& vStart, const Vector3i& vSize, sep::CellType eType, double dWallT);
  void FillInGrid(Config* pConfig);
  void LinkCells(Config* pConfig);
  void AddBox(Vector3i vStart, Vector3i vSize, Vector3b vWithoutFakes, bool bFlatZ, double dWallT, bool bGasBox);
  Cell* GetNeighb(Vector3i vCoord, sep::Axis eAxis, int iSlash);
  bool IsConer(Vector3i vStart, Vector3i vEnd, Vector3i vP);

  Grid* m_pGrid;
  std::vector<std::vector<std::vector<std::shared_ptr<InitCellData>>>> m_vCells;
};
