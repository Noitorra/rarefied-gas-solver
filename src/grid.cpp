#include "grid.h"
#include "grid_manager.h"

Grid::Grid() :
m_pGridManager(new GridManager()) {}

void Grid::Init() {
  m_pGridManager->SetParent(this);
}