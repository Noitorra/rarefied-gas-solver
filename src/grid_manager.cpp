#include "grid_manager.h"
#include "grid.h"
#include "cell.h"

GridManager::GridManager() {
  
}

void GridManager::SetParent(Grid* pGrid) {
  m_pGrid = pGrid;
}

void GridManager::SaveConfiguration(sep::Configuration config) {
  Build(config);
  std::string file_name = GenerateFileName(config);
  Write(file_name);
}

void GridManager::LoadConfiguration(sep::Configuration config) {
  std::string file_name = GenerateFileName(config);
  Read(file_name);
}

void GridManager::Build(sep::Configuration config) {
  std::cout << "Building grid" << std::endl;
}

bool GridManager::Write(const std::string& name) {
  std::cout << "Writing " << name << " on disk" << std::endl;
  return true;
}

bool GridManager::Read(const std::string& name) {
  std::cout << "Reading " << name << " from disk" << std::endl;
  std::shared_ptr<Cell> pCell(new Cell());
  m_pGrid->AddCell(pCell);
  return true;
}

const std::string GridManager::GenerateFileName(sep::Configuration config) const {
  std::string file_name("config/");
  switch (config) {
    case sep::DIMAN_CONFIG:
      file_name += std::string("diman");
      break;
    case sep::PROHOR_CONFIG:
      file_name += std::string("prohor");
      break;
    default:
      throw("SaveConfiguration: wrong config");
      break;
  }
  file_name += std::string(".grid");
  return file_name;
}