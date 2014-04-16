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

  Grid* GetGrid() const { return m_pGrid.get(); }
  Solver* GetSolver() const { return m_pSolver.get(); }
  Options* GetOptions() const { return m_pOptions.get(); }
  Config* GetConfig() const { return m_pConfig; }
  
private:
  void PushTemperature(double dT);
  double PopTemperature();
  void PushConcentration(double dConc);
  double PopConcentration();
  void SetLooping();
  void SetBox(Vector2i vP, Vector2i vSize);
  void SetVessel();
  void PrintGrid();
  void PreprocessGrid();
  void PostprocessGrid();
  void FindNeighbour(Vector2i vP, sep::CellType eType,
                     sep::NeighbType& eNeighb,
                     sep::Axis& eAxis, int& iQuant);
  bool GetNeighbour(Vector2i vP, sep::NeighbType eNeighb,
                    sep::Axis eAxis, sep::CellType& eType);
  int GetSlash(sep::NeighbType eType) const;
  
  std::vector<double> m_vTemperatureStack;
  std::vector<double> m_vConcetrationStack;
  std::shared_ptr<Grid> m_pGrid;
  std::shared_ptr<Solver> m_pSolver;
  std::shared_ptr<Options> m_pOptions;
  
  Config* m_pConfig;
};

#endif // GRID_MANGER_H_