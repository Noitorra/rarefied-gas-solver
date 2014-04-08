#include "out_results.h"
#include "grid.h"
#include "grid_manager.h"

void OutResults::Init(Grid* pGrid, GridManager* pGridManager) {
  m_pGrid = pGrid;
  m_pGridManager = pGridManager;
}

void OutResults::OutAll() {
  if (!m_pGrid || !m_pGridManager) {
    std::cout << "Error: member OutResults is not initialized yet" << std::endl;
    return;
  }
  LoadParameters();
  OutParameterSingletone(sep::T_PARAM);
  OutParameterSingletone(sep::C_PARAM);
}

// prepare parameters to be printed out
void OutResults::LoadParameters() {
  // Shouldn't we pass gas number to this function?
  
  std::vector<std::vector<std::vector<std::shared_ptr<InitCellData>>>>& m_vCells = m_pGridManager->m_vCells;
  
  const Vector3i& vSize = m_pGrid->GetSize();
  for (int x = 0; x < vSize.x(); x++) {
    for (int y = 0; y < vSize.y(); y++) {
      int z = iZLayer;
      std::shared_ptr<MacroParam>& pMacroPar = m_vCells[x][y][z]->m_pMacroParam;
      pMacroPar = std::shared_ptr<MacroParam>(new MacroParam());
      if (m_vCells[x][y][z]->m_eType != sep::NORMAL_CELL)
        continue;
    }
  }
}

void OutResults::OutParameterSingletone(sep::MacroParamType eType) {
  std::vector<std::vector<std::vector<std::shared_ptr<InitCellData>>>>& m_vCells = m_pGridManager->m_vCells;
  
  std::string filename;
  switch (eType) {
    case sep::T_PARAM:
      filename = "out/Temp.bin";
      break;
    case sep::C_PARAM:
      filename = "out/Conc.bin";
      break;
    default:
      // We are not able to print flow yet
      return;
  }
  std::ofstream fs(filename.c_str(), std::ios::out | std::ios::binary);
  
  const Vector3i& vSize = m_pGrid->GetSize();
  for (int x = 0; x < vSize.x(); x++) {
    for (int y = 0; y < vSize.y(); y++) {
      int z = iZLayer;
      double dParam = 0.0;
      switch (eType) {
        case sep::T_PARAM:
          dParam = m_vCells[x][y][z]->m_pMacroParam->m_dT;
          break;
        case sep::C_PARAM:
          dParam = m_vCells[x][y][z]->m_pMacroParam->m_dC;
          break;
        default:
          return;
      }
      fs.write(reinterpret_cast<const char*>(&dParam), sizeof(double));
    }
  }
  fs.close();
}

void OutResults::OutParameterMPI(sep::MacroParamType eType) {
  // TODO: To implement
}








