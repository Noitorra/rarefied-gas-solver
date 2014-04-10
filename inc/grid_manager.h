#pragma once
#include "main.h"
#include "types.h"

class Grid;
class Config;
class Cell;
class OutResults;
class MacroData;
class Solver;
class VesselGrid;

class InitCellData {
  friend class GridManager;
  friend OutResults;
protected:
  InitCellData(sep::CellType eType) :
  m_eType(eType),
  m_pCell(nullptr),
  m_bIsVesselCell(false),
  m_bIsVesselLeft(false),
  m_iVesselNumber(-1),
  m_bIsLoopedCell(false),
  m_bIsLoopedDown(false) {
    m_vMacroData.resize(2);
  };
  
  sep::CellType m_eType;
  Cell* m_pCell;
  std::vector<MacroData> m_vMacroData;
  MacroData m_cInitCond;
  bool m_bIsVesselCell;
  bool m_bIsVesselLeft; // Only for vessel cells
  int m_iVesselNumber; // The same
  bool m_bIsLoopedCell;
  bool m_bIsLoopedDown; // Only for looped cells
};

class GridManager {
  friend OutResults;
public:
  GridManager();
  void Init();
  void SetParent(Solver* pSolver);
  Grid* GetGrid() const { return m_pGrid.get(); }
  Solver* getSolver() { return m_pSolver; }
  OutResults* GetOutResults() const;
  Config* GetConfig() const;
  void BuildGrid();
  void Print(sep::Axis axis);
  const std::vector<std::shared_ptr<VesselGrid>>& GetLeftVessels() const {
    return m_vLeftVess;
  }
  const std::vector<std::shared_ptr<VesselGrid>>& GetRightVessels() const {
    return m_vRightVess;
  }
  const std::vector<std::shared_ptr<VesselGrid>>& GetLeftRightVessels(bool bIsLeft) const {
    if (bIsLeft)
      return m_vLeftVess;
    else
      return m_vRightVess;
  }
  
private:
  void SaveGridConfig(Config* pConfig);
  void LoadGridConfig(Config* pConfig);
  bool Write(const std::string& sName);
  bool Read(const std::string& sName);
  // Throws char const*
  const std::string GenerateFileName(sep::GridGeometry eGeometry) const;
  void Build(Config* pConfig);
  void BuildCombTypeGrid(Config* pConfig);
  void BuildHTypeGrid(Config* pConfig);
  void BuildDebugTypeGrid(Config* pConfig);
  void InitEmptyBox(const Vector3i& vSize);
  void SetBox(const Vector3i& vStart, const Vector3i& vSize, sep::CellType eType, double dWallT);
  void FillInGrid(Config* pConfig);
  void LinkCells(Config* pConfig);
  void AddBox(Vector3i vStart, Vector3i vSize, Vector3b vWithoutFakes, bool bFlatZ, double dWallT, bool bGasBox);
  Cell* GetNeighb(Vector3i vCoord, sep::Axis eAxis, int iSlash);
  bool IsConer(Vector3i vStart, Vector3i vEnd, Vector3i vP);
  
  void SetLoopedBox(const Vector3i& vStart, const Vector3i& vSize, bool bIsLoopedDown, double dT);
  void SetVesselBorderBox(const Vector3i& vStart, const Vector3i& vSize, bool bIsVesselLeft, int iVesselNumber, double dT);
  void InitVessels();

  std::shared_ptr<Grid> m_pGrid;
  std::shared_ptr<OutResults> m_pOutResults;
  Solver* m_pSolver;
  std::vector<std::vector<std::vector<std::shared_ptr<InitCellData>>>> m_vCells;
  
  std::vector<std::shared_ptr<VesselGrid>> m_vRightVess;
  std::vector<std::shared_ptr<VesselGrid>> m_vLeftVess;
};
