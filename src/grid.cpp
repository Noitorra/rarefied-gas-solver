#include "grid.h"
#include "grid_manager.h"
#include "solver.h"

Grid::Grid() :
m_pGridManager(new GridManager()),
m_pSolver(nullptr) {}

void Grid::Init() {
  m_pGridManager->SetParent(this);
}

void Grid::AddCell(std::shared_ptr<Cell> pCell) {
  m_vCells.push_back(pCell);
}

GridManager* Grid::GetGridManager() const {
  return m_pGridManager.get();
}

void Grid::BuildWithActiveConfig() {
  try {
    m_pGridManager->SaveGridConfig(m_pSolver->GetActiveConfig());
  } catch (char const* sExc) {
    std::cout << "Exception occurs: " << sExc << std::endl;
    return;
  }
}