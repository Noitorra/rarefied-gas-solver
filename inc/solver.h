#ifndef SOLVER_H_
#define SOLVER_H_

#include "main.h"
#include <memory>

class Parallel;
class SolverInfo;
class Options;
class Grid;
class GridManager;
class Config;

class Solver {
public:
  Solver();
  virtual ~Solver();

  void Init();
  void Run();
  
  Parallel* getParallel() const { return m_pParallel.get(); }
	SolverInfo* getSolverInfo() const { return m_pSolverInfo.get(); }
	Grid* GetGrid() const;
  // Throws const char*
  Config* GetConfig() const;
  
private:
  std::shared_ptr<Parallel> m_pParallel;
	std::shared_ptr<SolverInfo> m_pSolverInfo;
  std::shared_ptr<GridManager> m_pGridManager;
  Grid* m_pGrid;

  void initCellType(sep::Axis axis);
  void makeStep(sep::Axis axis);
  void makeIntegral(unsigned int gi0, unsigned int gi1, double timestep);
};

#endif /* SOLVER_H_ */
