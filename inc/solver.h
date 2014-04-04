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
public:
  Solver();
  void Init();
  
private:
  class Grid* m_pSolver;
  class Grid* m_pGrid;
};
