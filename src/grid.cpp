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

void Grid::PrintCellsLinkage() {
  if (m_vCells.empty()) {
    return;
  }
  Cell* first_cell = m_vCells[1].get();
  Cell* cell = first_cell;
  Cell* cell_y = first_cell;
  
  while (true) {
    while (true) {
      std::cout << cell->m_vType[0] << " ";
      if (cell->m_vNext[sep::X].empty())
        break;
      cell = cell->m_vNext[sep::X][0];
    }
    std::cout << std::endl;
    if (cell_y->m_vNext[sep::Y].empty())
      break;
    cell_y = cell_y->m_vNext[sep::Y][0];
    cell = cell_y;
  }
}




