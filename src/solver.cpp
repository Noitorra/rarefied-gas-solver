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

Solver::Solver() :
m_pGrid(new Grid()) {}

void Solver::Init() {
  m_pGrid->SetParent(this);
  m_pGrid->Init();
}
