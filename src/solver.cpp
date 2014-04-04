#include "solver.h"
#include "grid.h"

Solver::Solver() :
m_pGrid(new Grid()) {}

void Solver::Init() {
  m_pGrid->SetParent(this);
  m_pGrid->Init();
}