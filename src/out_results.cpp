#include "out_results.h"
#include "grid.h"
#include "grid_manager.h"
#include "cell.h"
#include "config.h"
#include "vessel_grid.h"
#include "gas.h"
#include "solver.h"
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

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
    OutParameterSingletone(sep::P_PARAM, iGas, iIteration);
  }
  OutAverageStream(iIteration);
}

// prepare parameters to be printed out
void OutResults::LoadParameters() {
  std::vector<std::vector<std::vector<std::shared_ptr<InitCellData>>>>& m_vCells = m_pGrid->GetInitCells();
  
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
  
  const std::vector<std::shared_ptr<VesselGrid>>& vVessels = m_pGrid->GetVessels();
  
  for (auto& pVessel : vVessels) {
    pVessel->computeMacroData();
  }

}

void OutResults::OutParameterSingletone(sep::MacroParamType eType, int iGas, int iIndex) {
  std::vector<std::vector<std::vector<std::shared_ptr<InitCellData>>>>& m_vCells = m_pGrid->GetInitCells();
  Config* pConfig = m_pGridManager->GetConfig();
  
  std::string filename;
  const std::string& sOutputPrefix = pConfig->GetOutputPrefix();
  switch (eType) {
    case sep::T_PARAM:
      filename = sOutputPrefix + "out/gas" + std::to_string(iGas) + "/temp/" + std::to_string(iIndex) + ".bin";
      break;
    case sep::C_PARAM:
      filename = sOutputPrefix + "out/gas" + std::to_string(iGas) + "/conc/" + std::to_string(iIndex) + ".bin";
      break;
    case sep::P_PARAM:
      filename = sOutputPrefix + "out/gas" + std::to_string(iGas) + "/pressure/" + std::to_string(iIndex) + ".bin";
      break;
    default:
      // We are not able to print flow yet
      return;
  }
  std::ofstream fs(filename.c_str(), std::ios::out | std::ios::binary);
  
  const Vector3i& vGridSize = pConfig->GetGridSize();
  const Vector3i& vStartOutGrid = pConfig->GetOutputGridStart();
  const Vector3i& vOutputSize = pConfig->GetOutputSize();
  
  for (int y = 0; y < vOutputSize.y(); y++) {
    for (int x = 0; x < vOutputSize.x(); x++) {
      int z = 0;
      double dParam = 0.0;
      
      int g_x = x - vStartOutGrid.x();
      int g_y = y - vStartOutGrid.y();
      int g_z = z - vStartOutGrid.z();
      // Edge cells are faked
      int iEdge = 0;
      // Out grid
      if (g_x >= 0 + iEdge && g_y >= 0 + iEdge && g_z >= 0 + iEdge &&
          g_x < vGridSize.x() - iEdge && g_y < vGridSize.y() - iEdge && g_z < vGridSize.z() - iEdge) {
        
        Cell* cell = m_vCells[g_x][g_y][g_z]->m_pCell;
        if (!cell) {
          dParam = 0.0;
        } else {
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
              case sep::P_PARAM:
                dParam = cell->m_vMacroData[iGas].Concentration * cell->m_vMacroData[iGas].Temperature;
                goto next_cell_label;
              default:
                return;
            }
          }
        }
      } else {
        // Looking for vessels
        
        // For all vessels
        const std::vector<std::shared_ptr<VesselGrid>>& vVessels = m_pGrid->GetVessels();
          
        for (auto& pVessel : vVessels) {

          const std::vector< std::vector<Cell*> >& vVessCells = pVessel->GetPrintVector();
          const Vector2i& vVesselSize2D = pVessel->GetPrintVectorSize();
          Vector3i vVesselSize(vVesselSize2D.x(), vVesselSize2D.y(), 1);
          Vector3i vVesselStart = pVessel->getVesselGridInfo()->vStart;
//          if (iLeftVess)
//            vVesselStart.x() = vStartOutGrid.x() - vVesselSize.x();
//          else
//            vVesselStart.x() = vStartOutGrid.x() + vGridSize.x();
          
          int v_x = x - vVesselStart.x();
          int v_y = y - vVesselStart.y();
          int v_z = z - vVesselStart.z();
          
          iEdge = 0;
          if (v_x >= 0 + iEdge && v_y >= 0 + iEdge && v_z >= 0 + iEdge &&
              v_x < vVesselSize.x() - iEdge && v_y < vVesselSize.y() - iEdge && v_z < vVesselSize.z() - iEdge) {
            
            Cell* cell = nullptr;
//            if (!iLeftVess)
//              cell = vVessCells[vVesselSize.x() - v_x - 1][v_y];
//            else
//              cell = vVessCells[v_x][v_y];

            if (!cell) {
              dParam = 0.0;
              goto next_cell_label;
            }
            else {
              if ((cell->m_vType[sep::X] == Cell::CT_NORMAL ||
                   cell->m_vType[sep::X] == Cell::CT_PRERIGHT) &&
                  (cell->m_vType[sep::Y] == Cell::CT_NORMAL ||
                   cell->m_vType[sep::Y] == Cell::CT_PRERIGHT)) {
//                if (true) {
                switch (eType) {
                  case sep::T_PARAM:
                    dParam = cell->m_vMacroData[iGas].Temperature;
                    goto next_cell_label;
                    break;
                  case sep::C_PARAM:
                    dParam = cell->m_vMacroData[iGas].Concentration;
                    goto next_cell_label;
                    break;
                  case sep::P_PARAM:
                    dParam = cell->m_vMacroData[iGas].Concentration * cell->m_vMacroData[iGas].Temperature;
                    goto next_cell_label;
                    break;
                  default:
                    return;
                }
              } else {
                dParam = 0.0;
              }
            }
          }
        }
        dParam = 0.0;
      }
      next_cell_label:
      fs.write(reinterpret_cast<const char*>(&dParam), sizeof(double));
    }
  }
  fs.close();
}

void OutResults::OutParameterMPI(sep::MacroParamType eType) {
  // TODO: To implement
}

void OutResults::OutAverageStream(int iIteration) {
  GasVector& gasv = m_pGridManager->GetSolver()->GetGas();
  Config* pConfig = m_pGridManager->GetConfig();

  const std::string& sOutputPrefix = pConfig->GetOutputPrefix();
  std::string sASFilenameBase = sOutputPrefix + "out/gas";
  for (unsigned int gi = 0; gi < gasv.size(); gi++) {
    std::string sASFilename = sASFilenameBase + std::to_string(gi);
    sASFilename += "/";
    sASFilename += "average_stream.bin";

    std::fstream test;
    test.open("test.bin", std::ios::out | std::ios::binary);
    test.close();

    namespace fs = boost::filesystem;
    fs::path full_path( fs::current_path() );
    std::cout << "Current path is : " << full_path << std::endl;



    std::fstream filestream;
    std::ios::openmode openmode;
    if (iIteration == 0) {
      openmode = std::ios::out | std::ios::binary;
    }
    else {
      openmode = std::ios::app | std::ios::binary;
    }
    filestream.open(sASFilename, openmode);
    if (filestream.is_open()) {

      switch (pConfig->GetGridGeometryType()) {
        case sep::COMB_GRID_GEOMETRY:
          OutAverageStreamComb(filestream, gi);
          break;
        case sep::H_GRID_GEOMTRY:
          OutAverageStreamHType(filestream, gi);
          break;
        default:
          return;
      }
      filestream.close();
    }
    else {
      std::cout << "OutResults::OutAverageStream() : Cannot open file - " << sASFilename << std::endl;
    }
  }
}

void OutResults::OutAverageStreamComb(std::fstream& filestream, int iGasN) {
  const Vector3i& vSize = m_pGrid->GetSize();
  double dLeftAverageStream = ComputeAverageColumnStream(1, iGasN, 0, vSize.y());
  double dRightAverageStream = ComputeAverageColumnStream(vSize.x() - 2, iGasN, 0, vSize.y());
  
  filestream.write(reinterpret_cast<const char*>(&dLeftAverageStream), sizeof(double));
  filestream.write(reinterpret_cast<const char*>(&dRightAverageStream), sizeof(double));
  
  std::cout << dLeftAverageStream << " : " << dRightAverageStream << std::endl;
}

void OutResults::OutAverageStreamHType(std::fstream& filestream, int iGasN) {
  const Vector3i& vSize = m_pGrid->GetSize();
  Config* pConfig = m_pGridManager->GetConfig();
  HTypeGridConfig* pHTypeConfig = pConfig->GetHTypeGridConfig();
  int iShiftX = 5;
  int D = pHTypeConfig->D;
  int l = pHTypeConfig->l;
  
  double dLeftUpStream = ComputeAverageColumnStream(iShiftX, iGasN, 1, D - 2);
  double dRightUpStream = ComputeAverageColumnStream(vSize.x() - 1 - iShiftX, iGasN, 1, D - 2);
  double dLeftDownStream = ComputeAverageColumnStream(iShiftX, iGasN, D + l + 1, D - 2);
  double dRightDownStream = ComputeAverageColumnStream(vSize.x() - 1 - iShiftX, iGasN, D + l + 1, D - 2);
  
  filestream.write(reinterpret_cast<const char*>(&dLeftUpStream), sizeof(double));
  filestream.write(reinterpret_cast<const char*>(&dRightUpStream), sizeof(double));
  filestream.write(reinterpret_cast<const char*>(&dLeftDownStream), sizeof(double));
  filestream.write(reinterpret_cast<const char*>(&dRightDownStream), sizeof(double));
  
  std::cout << dLeftUpStream << " : " << dRightUpStream << " : " <<
  dLeftDownStream << " : " << dRightDownStream << std::endl;
}

double OutResults::ComputeAverageColumnStream(int iIndexX, unsigned int gi, int iStartY, int iSizeY) {
  std::vector<std::vector<std::vector<std::shared_ptr<InitCellData>>>>& m_vCells = m_pGrid->GetInitCells();

  double dAverageStream = 0.0;
  for (int y = iStartY; y < iStartY + iSizeY; y++) {
    dAverageStream += m_vCells[iIndexX][y][0]->m_vMacroData[gi].Stream.x();
  }

  dAverageStream /= iSizeY;
 
  return dAverageStream;
}