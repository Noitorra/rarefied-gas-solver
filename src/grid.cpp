#include "grid.h"
#include "grid_manager.h"
#include "solver.h"
#include "out_results.h"
#include "cell.h"

Grid::Grid() {}

void Grid::Init(GridManager* pGridManager) {}

void Grid::AddCell(std::shared_ptr<Cell> pCell) {
  m_vCells.push_back(pCell);
}

std::vector<std::shared_ptr<Cell>>& Grid::GetCells() {
	return m_vCells;
}

std::vector<std::vector<std::vector<std::shared_ptr<InitCellData>>>>& Grid::GetInitCells() {
	return m_vInitCells;
}

std::vector<std::shared_ptr<VesselGrid>>& Grid::GetVessels() {
  return m_vVessels;
}







