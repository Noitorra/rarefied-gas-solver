#include "grid.h"
#include "grid_manager.h"

Grid::Grid() :
m_pGridManager(new GridManager()) {}

void Grid::Init() {
  m_pGridManager->SetParent(this);
}

void Grid::AddCell(std::shared_ptr<Cell> pCell) {
  m_vCells.push_back(pCell);
}

GridManager* Grid::GetGridManager() const {
  return m_pGridManager.get();
}

void Grid::SaveConfiguration(sep::Configuration config) {
  m_pGridManager->SaveConfiguration(config);
}

void Grid::LoadConfiguration(sep::Configuration config) {
  m_pGridManager->LoadConfiguration(config);
}