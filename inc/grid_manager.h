#ifndef GRID_MANGER_H_
#define GRID_MANGER_H_

#include "main.h"
#include "grid_constructor.h"

class Grid;
class Solver;
class Options;
class Config;

class GridManager : public GridConstructor {
public:
  GridManager();
  void Init();
  void ConfigureGrid();
  void PrintLinkage(sep::Axis eAxis);

  Grid* GetGrid() const { return m_pGrid.get(); }
  Solver* GetSolver() const { return m_pSolver.get(); }
  Options* GetOptions() const { return m_pOptions.get(); }
  Config* GetConfig() const { return m_pConfig; }
  
private:
  void PushTemperature(double dT);
  double PopTemperature();
  double GetTemperature();
  void PushConcentration(double dConc);
  double PopConcentration();
  double GetConcentration();
  void SetLooping();
  void SetBox(Vector2i vP, Vector2i vSize);
  void SetVessel();
  void PrintGrid();
  void PreprocessGrid();
  void PostprocessGrid();
  void FindNeighbour(Vector2i vP, sep::CellType eType,
                     sep::Axis& eAxis,
                     sep::NeighbType& eNeighb,
                     int& iQuant);
  bool FindNeighbourWithIndex(Vector2i vP,
                              sep::CellType eType,
                              int iIndex,
                              sep::Axis& eAxis,
                              sep::NeighbType& eNeighb);
  bool GetNeighbour(Vector2i vP, sep::Axis eAxis,
                    sep::NeighbType eNeighb,
                    sep::CellType& eType);
  int GetSlash(sep::NeighbType eType) const;
  void FillInGrid();
  void LinkCells();
  void InitCells();
  void LinkNeighb(Vector2i vP, sep::Axis eAxis,
                  sep::NeighbType eNeighb);
  
  std::vector<double> m_vTemperatureStack;
  std::vector<double> m_vConcetrationStack;
  std::shared_ptr<Grid> m_pGrid;
  std::shared_ptr<Solver> m_pSolver;
  std::shared_ptr<Options> m_pOptions;
  
  Config* m_pConfig;
};

#endif // GRID_MANGER_H_