#include "grid.h"
#include "grid_manager.h"
#include "solver.h"
#include "out_results.h"
#include "cell.h"
#include "config.h"

Grid::Grid() {}

void Grid::Init(GridManager* pGridManager) {
  Config* pConfig = pGridManager->GetConfig();
  m_vSize = pConfig->GetGridSize();
  m_vStart = Vector3i();
  m_vWholeSize = m_vSize;
  
  AllocateInitData();
}

void Grid::AddCell(std::shared_ptr<Cell> pCell) {
  m_vCells.push_back(pCell);
}

std::vector<std::shared_ptr<Cell>>& Grid::GetCells() {
	return m_vCells;
}

std::vector<std::vector<std::vector<std::shared_ptr<InitCellData>>>>& Grid::GetInitCells() {
	return m_vInitCells;
}

InitCellData* Grid::GetInitCell(Vector2i vP) {
  if (vP.x() < 0 || vP.y() < 0 ||
      vP.x() >= m_vSize.x() || vP.y() >= m_vSize.y()) {
    throw ("Wrong position");
  }
  return m_vInitCells[vP.x()][vP.y()][0].get();
}

std::vector<std::shared_ptr<VesselGrid>>& Grid::GetVessels() {
  return m_vVessels;
}

void Grid::AllocateInitData() {
  for (int x = 0; x < m_vSize.x(); x++) {
    std::vector<std::vector<std::shared_ptr<InitCellData>> > vec_yz;
    m_vInitCells.push_back(vec_yz);
    
    for (int y = 0; y < m_vSize.y(); y++) {
      std::vector<std::shared_ptr<InitCellData>> vec_z;
      m_vInitCells[x].push_back(vec_z);
      
      for (int z = 0; z < m_vSize.z(); z++) {
        m_vInitCells[x][y].push_back(std::shared_ptr<InitCellData>(new InitCellData(sep::FAKE_CELL))); // Init fake cells, it's ok
        // to see more go to grid_manager.cpp
      }
    }
  }
}







