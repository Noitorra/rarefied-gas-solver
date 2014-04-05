#ifndef SOLVER_H_
#define SOLVER_H_

#include <memory>

class Parallel;
class SolverInfo;
class Grid;

class Solver {
public:
  Solver();
  virtual ~Solver();
  void Init();
  
  Parallel* getParallel() const { return m_pParallel.get(); }
	SolverInfo* getSolverInfo() const { return m_pSolverInfo.get(); }
	Grid* GetGrid() const;
  
private:
  std::shared_ptr<Parallel> m_pParallel;
	std::shared_ptr<SolverInfo> m_pSolverInfo;

  std::shared_ptr<Grid> m_pGrid;
};

#endif /* SOLVER_H_ */