#include "out_results.h"
#include "grid.h"
#include "grid_manager.h"
#include "cell.h"
#include "config.h"
#include "vessel_grid.h"

void OutResults::Init(Grid* pGrid, GridManager* pGridManager) {
  m_pGrid = pGrid;
  m_pGridManager = pGridManager;
}

void OutResults::OutAll(int iIteration) {
  if (!m_pGrid || !m_pGridManager) {
    std::cout << "Error: member OutResults is not initialized yet" << std::endl;
    return;
  }
  LoadParameters();
  for (int iGas = 0; iGas < 2; iGas++) {
    OutParameterSingletone(sep::T_PARAM, iGas, iIteration);
    OutParameterSingletone(sep::C_PARAM, iGas, iIteration);
  }
}

// prepare parameters to be printed out
void OutResults::LoadParameters() {
  std::vector<std::vector<std::vector<std::shared_ptr<InitCellData>>>>& m_vCells = m_pGridManager->m_vCells;
  
  const Vector3i& vSize = m_pGrid->GetSize();
  for (int x = 0; x < vSize.x(); x++) {
    for (int y = 0; y < vSize.y(); y++) {
      int z = iZLayer;
      if (m_vCells[x][y][z]->m_eType != sep::NORMAL_CELL)
        continue;
      Cell* cell = m_vCells[x][y][z].get()->m_pCell;
      cell->computeMacroData();
      const std::vector<MacroData>& vMacroData = cell->getMacroData();
      m_vCells[x][y][z]->m_vMacroData = vMacroData;
    }
  }
}

void OutResults::OutParameterSingletone(sep::MacroParamType eType, int iGas, int iIndex) {
  std::vector<std::vector<std::vector<std::shared_ptr<InitCellData>>>>& m_vCells = m_pGridManager->m_vCells;
  Config* pConfig = m_pGridManager->GetConfig();
  
  std::string filename;
  switch (eType) {
    case sep::T_PARAM:
      filename = "../out/gas" + std::to_string(iGas) + "/temp/" + std::to_string(iIndex) + ".bin";
      break;
    case sep::C_PARAM:
      filename = "../out/gas" + std::to_string(iGas) + "/den/" + std::to_string(iIndex) + ".bin";
      break;
    default:
      // We are not able to print flow yet
      return;
  }
  std::ofstream fs(filename.c_str(), std::ios::out | std::ios::binary);
  
  const Vector3i& vGridSize = m_pGrid->GetSize();
  const Vector3i& vStartOutGrid = pConfig->GetOutputGridStart();
  const Vector3i& vOutputSize = pConfig->GetOutputSize();
  
  // Left vessel
//  const Vector3i& vLVesselStart = m_pGridManager->m_vLeftVess[0]->getVesselGridInfo()->vStart;
//  const Vector3i& vLVesselSize = m_pGridManager->m_vLeftVess[0]->getVesselGridInfo()->vSize;
  
  // Edge cells are faked
  for (int y = 0; y < vOutputSize.y(); y++) {
    for (int x = 0; x < vOutputSize.x(); x++) {
      int z = iZLayer;
      double dParam = 0.0;
      
      int g_x = x - vStartOutGrid.x();
      int g_y = y - vStartOutGrid.y();
      int g_z = z - vStartOutGrid.z();
      dParam = 0.0;
      int iEdge = 0;
      // Out grid
      if (g_x >= 0 + iEdge && g_y >= 0 + iEdge && g_z >= 0 + iEdge &&
          g_x < vGridSize.x() - iEdge && g_y < vGridSize.y() - iEdge && g_z < vGridSize.z() - iEdge) {
        if (m_vCells[g_x][g_y][g_z]->m_eType != sep::NORMAL_CELL) {
          dParam = 0.0;
        } else {
          switch (eType) {
            case sep::T_PARAM:
              dParam = m_vCells[g_x][g_y][g_z]->m_vMacroData[iGas].Temperature;
              break;
            case sep::C_PARAM:
              dParam = m_vCells[g_x][g_y][g_z]->m_vMacroData[iGas].Concentration;
              break;
            default:
              return;
          }
        }
      } else {
        // Looking for vessels
        
        // Left vessel
        
      }
//      std::cout << "(" << x << "," << y << "," << z << "): " <<
//      dParam << std::endl;
      fs.write(reinterpret_cast<const char*>(&dParam), sizeof(double));
    }
  }
  fs.close();
}

void OutResults::OutParameterMPI(sep::MacroParamType eType) {
  // TODO: To implement
}








