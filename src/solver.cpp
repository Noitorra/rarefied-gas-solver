#include "solver.h"

#include "parallel.h"
#include "solver_info.h"
#include "options.h"
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

void Solver::Init() {
	// We must OBEY order:
	// first info (impulse, etc)
	// second grid....
  m_pSolverInfo->setSolver(this);
  m_pSolverInfo->Init();

  // Init grid
  m_pGrid->SetParent(this);
  m_pGrid->Init();
}

void Solver::Run() {

}

Config* Solver::GetActiveConfig() const {
  return m_pSolverInfo->getOptions()->GetActiveConfig();
}
