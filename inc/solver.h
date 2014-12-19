#ifndef SOLVER_H_
#define SOLVER_H_

#include "main.h"
#include <memory>

class Parallel;
class Grid;
class Impulse;
class Gas;
class GridManager;
class Solver;



class Solver {
public:
  Solver();

  void Init(GridManager* pGridManager);
  void Run();
  
  Parallel* GetParallel() const { return m_pParallel.get(); }
	Impulse* GetImpulse() const { return m_pImpulse.get(); }

private:
  void InitCellType(sep::Axis axis);
  void MakeStep(sep::Axis axis);
  void MakeIntegral(unsigned int gi0, unsigned int gi1, double timestep);
  void PreRun();
  
  std::shared_ptr<Parallel> m_pParallel;
	std::shared_ptr<Impulse> m_pImpulse;
  
  GridManager* m_pGridManager;
  Grid* m_pGrid;
};

#endif /* SOLVER_H_ */
