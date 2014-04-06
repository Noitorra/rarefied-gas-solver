#include "solver.h"

#include "parallel.h"
#include "solver_info.h"
#include "options.h"
#include "grid.h"

Solver::Solver()
: m_pParallel(new Parallel),
  m_pSolverInfo(new SolverInfo),
  m_pOptions(new Options),
  m_pGrid(new Grid)
{
	// TODO Auto-generated constructor stub
}

void Solver::Init() {
  // Init options
  m_pOptions->Init();
  
  // Init grid
  m_pGrid->SetParent(this);
  m_pGrid->Init();
}

Solver::~Solver() {
	// TODO Auto-generated destructor stub
}

Config* Solver::GetActiveConfig() const { return m_pOptions->GetActiveConfig(); }
