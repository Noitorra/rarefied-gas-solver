#ifndef SOLVER_H_
#define SOLVER_H_

#include <memory>

class Parallel;
class SolverInfo;
class Grid;

class Solver {
private:

public:
  Solver();
  virtual ~Solver();
  void Init();
  
  Parallel* getParallel() { return m_pParallel.get(); }
	SolverInfo* getSolverInfo() { return m_pSolverInfo.get(); }
	Grid* getGrid() { return m_pGrid.get(); }
  
private:
  std::shared_ptr<Parallel> m_pParallel;
	std::shared_ptr<SolverInfo> m_pSolverInfo;

  std::shared_ptr<class Grid> m_pGrid;
};

#endif /* SOLVER_H_ */