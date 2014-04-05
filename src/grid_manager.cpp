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

void GridManager::Build(Config* pConfig) {
  std::cout << "Building grid" << std::endl;
}

bool GridManager::Write(const std::string& sName) {
  std::cout << "Writing " << sName << " on disk" << std::endl;
  return true;
}

bool GridManager::Read(const std::string& sName) {
  std::cout << "Reading " << sName << " from disk" << std::endl;
  std::shared_ptr<Cell> pCell(new Cell());
  m_pGrid->AddCell(pCell);
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