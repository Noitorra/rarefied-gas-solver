#include "grid.h"
#include "grid_manager.h"
#include "solver.h"
#include "out_results.h"
#include "cell.h"

Grid::Grid() :
m_pGridManager(new GridManager()),
m_pOutResults(new OutResults()),
m_pSolver(nullptr) {}

void Grid::Init() {
  m_pGridManager->SetParent(this);
  m_pOutResults->Init(this, m_pGridManager.get());
}

void Grid::AddCell(std::shared_ptr<Cell> pCell) {
  m_vCells.push_back(pCell);
}

std::vector<std::shared_ptr<Cell>>& Grid::getCellVector() {
	return m_vCells;
}

GridManager* Grid::GetGridManager() const {
  return m_pGridManager.get();
}
OutResults* Grid::GetOutResults() const {
  return m_pOutResults.get();
}
Config* Grid::GetConfig() const {
  return m_pSolver->GetConfig();
}

void Grid::BuildWithActiveConfig() {
  try {
    m_pGridManager->SaveGridConfig(GetConfig());
  } catch (char const* sExc) {
    std::cout << "Exception occurs: " << sExc << std::endl;
    return;
  }
}




