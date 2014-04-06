#include "grid_manager.h"
#include "grid.h"
#include "cell.h"
#include "config.h"

GridManager::GridManager() {
  
}

void GridManager::SetParent(Grid* pGrid) {
  m_pGrid = pGrid;
}

void GridManager::SaveGridConfig(Config* pConfig) {
  Build(pConfig);
  std::string file_name;
  try {
    file_name = GenerateFileName(pConfig->GetGridGeometryType());
  } catch (char const* sExc) {
    std::cout << "Exception occurs: " << sExc << std::endl;
    return;
  }
  Write(file_name);
}

void GridManager::LoadGridConfig(Config* pConfig) {
  std::string file_name;
  try {
    file_name = GenerateFileName(pConfig->GetGridGeometryType());
  } catch (char const* sExc) {
    std::cout << "Exception occurs: " << sExc << std::endl;
    return;
  }
  Read(file_name);
}

bool GridManager::Write(const std::string& sName) {
  // TODO: To implement
  return true;
}

bool GridManager::Read(const std::string& sName) {
  // TODO: To implement
  return true;
}

const std::string GridManager::GenerateFileName(sep::GridGeometry eGeometry) const {
  std::string file_name("config/");
  switch (eGeometry) {
    case sep::DIMAN_GRID_GEOMETRY:
      file_name += std::string("diman");
      break;
    case sep::PROHOR_GRID_GEOMTRY:
      file_name += std::string("prohor");
      break;
    default:
      throw("SaveConfiguration: wrong config");
      break;
  }
  file_name += std::string(".grid");
  return file_name;
}

void GridManager::Build(Config* pConfig) {
  std::cout << "Building grid" << std::endl;
}

void GridManager::SetBox(const Vector3i& vStart, const Vector3i& vSize, sep::CellType eType) {
  for (int x = vStart.x(); x < vStart.x() + vSize.x(); x++) {
    std::vector<std::vector<std::shared_ptr<InitCellData>> > vec_yz;
    m_vCells.push_back(vec_yz);
    
    for (int y = vStart.y(); y < vStart.y() + vSize.y(); y++) {
      std::vector<std::shared_ptr<InitCellData>> vec_z;
      m_vCells[x].push_back(vec_z);
      
      for (int z = vStart.z(); z < vStart.z() + vSize.z(); z++) {
        m_vCells[x][y].push_back(std::shared_ptr<InitCellData>(new InitCellData(sep::EMPTY_CELL)));
      }
    }
  }
}








