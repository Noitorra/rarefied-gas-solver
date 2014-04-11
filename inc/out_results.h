#pragma once

#include "main.h"
#include "types.h"

class Grid;
class GridManager;


class OutResults {
public:
  OutResults() :
  m_pGrid(nullptr),
  m_pGridManager(nullptr),
  iZLayer(0) // For a while
  {};
  void Init(Grid* pGrid, GridManager* pGridManager);
  void OutAll(int iIteration);
  
  void OutAverageStream(int iIteration);
private:
  void LoadParameters();
  void OutParameterSingletone(sep::MacroParamType eType, int iGas, int iIndex);
  void OutParameterMPI(sep::MacroParamType eType);

  double ComputeAverageColumnStream(int iIndexX, unsigned int gi, int iStartY, int iSizeY);
  
  void OutAverageStreamComb(std::fstream& filestream, int iGasN);
  void OutAverageStreamHType(std::fstream& filestream, int iGasN);
  
  Grid* m_pGrid;
  GridManager* m_pGridManager;
  int iZLayer;
};