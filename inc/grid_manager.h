#ifndef GRID_MANGER_H_
#define GRID_MANGER_H_

#include "main.h"

class Grid;
class Solver;
class Options;
class Config;

class GridManager {
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
  //  SetLooping();
  void SetBox();
  //  PushConcentration();
  //  PopConcentration();
  //  SetVessel();
  
  std::vector<double> m_TemperatureStack;
  std::shared_ptr<Grid> m_pGrid;
  std::shared_ptr<Solver> m_pSolver;
  std::shared_ptr<Options> m_pOptions;
  
  Config* m_pConfig;
};

#endif // GRID_MANGER_H_