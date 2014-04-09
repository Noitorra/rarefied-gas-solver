#include "grid.h"
#include "grid_manager.h"
#include "solver.h"
#include "out_results.h"
#include "cell.h"

Grid::Grid() {}

void Grid::Init() {}

void Grid::AddCell(std::shared_ptr<Cell> pCell) {
  m_vCells.push_back(pCell);
}

std::vector<std::shared_ptr<Cell>>& Grid::getCellVector() {
	return m_vCells;
}

GridManager* Grid::GetGridManager() const {
  return m_pGridManager;
}






