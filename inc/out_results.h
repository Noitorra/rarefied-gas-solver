#pragma once

#include "main.h"
#include "types.h"

class Grid;
class GridManager;

class MacroParam {
  friend class OutResults;
private:
  MacroParam() :
  m_dT(0.0), m_dC(0.0) {};
  
  void Set(double dT, double dC, Vector2d vFlow) {
    m_dT = dT;
    m_dC = dC;
    m_vFlow = vFlow;
  }
  double m_dT; // Temperature
  double m_dC; // Concentraion
  Vector2d m_vFlow; // Flow
};

class OutResults {
public:
  OutResults() :
  m_pGrid(nullptr),
  m_pGridManager(nullptr),
  iZLayer(0) // For a while
  {};
  void Init(Grid* pGrid, GridManager* pGridManager);
  void OutAll();
  
private:
  void LoadParameters();
  void OutParameterSingletone(sep::MacroParamType eType);
  void OutParameterMPI(sep::MacroParamType eType);
  
  Grid* m_pGrid;
  GridManager* m_pGridManager;
  int iZLayer;
};