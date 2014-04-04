/*
 * solver.h
 *
 *  Created on: 04 апр. 2014 г.
 *      Author: kisame
 */

#ifndef SOLVER_H_
#define SOLVER_H_

#include <memory>

class Parallel;
class SolverInfo;
class Grid;

class Solver {
private:
	std::shared_ptr<Parallel> m_pParallel;
	std::shared_ptr<SolverInfo> m_pSolverInfo;
	std::shared_ptr<Grid> m_pGrid;
public:
	Solver();
	virtual ~Solver();

	Parallel* getParallel() { return m_pParallel.get(); }
	SolverInfo* getSolverInfo() { return m_pSolverInfo.get(); }
	Grid* getGrid() { return m_pGrid.get(); }
};

#endif /* SOLVER_H_ */
