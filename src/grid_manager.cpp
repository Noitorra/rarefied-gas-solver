#include "grid_manager.h"
#include "grid.h"

GridManager::GridManager() {
  
}

void GridManager::SetParent(Grid* pGrid) {
  m_pGrid = pGrid;
  std::cout << "GridManager have parent!" << std::endl;
}

bool GridManager::Write(const std::string& name) {
  return true;
}

bool GridManager::Read(const std::string& name) {
  return true;
}