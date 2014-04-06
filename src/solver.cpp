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

void Solver::Init() {  
  // Init grid
  m_pGrid->SetParent(this);
  m_pGrid->Init();
  
  m_pSolverInfo->Init();
}

Solver::~Solver() {
	// TODO Auto-generated destructor stub
}

Config* Solver::GetActiveConfig() const {
  return m_pSolverInfo->getOptions()->GetActiveConfig();
}
