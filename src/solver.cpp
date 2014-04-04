<<<<<<< HEAD
/*
 * solver.cpp
 *
 *  Created on: 04 апр. 2014 г.
 *      Author: kisame
 */

#include "solver.h"

#include "parallel.h"
#include "solver_info.h"
#include "grid.h"

Solver::Solver()
: m_pParallel(new Parallel),
  m_pSolverInfo(new SolverInfo),
  m_pGrid(new Grid)
{
	// TODO Auto-generated constructor stub
}

Solver::~Solver() {
	// TODO Auto-generated destructor stub
}
