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

void Solver::Init() {
  m_pGrid->SetParent(this);
  m_pGrid->Init();
}

Solver::~Solver() {
	// TODO Auto-generated destructor stub
}