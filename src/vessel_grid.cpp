#include "vessel_grid.h"
#include "cell.h"
#include "grid_manager.h"

#include <exception>

void VesselGrid::CreateAndLinkVessel() {
  try {
    switch (m_eType) {
    case VGT_NORMAL:
      if (compute_binary_log(m_pVGInfo->iNy - 2) == -1) {
        throw std::exception("Wrong iNy, needed 2^n + 2");
      }
      create_normal_vessel();
      link_normal_vessel();
      generate_normal_print_vector();
      break;
    case VGT_CYCLED:
      if (compute_binary_log(m_pVGInfo->iNy) == -1) {
        throw std::exception("Wrong iNy, needed 2^n");
      }
      create_cycled_vessel();
      link_cycled_vessel();
      generate_cycled_print_vector();
      break;
    }
  }
  catch (std::exception& e) {
    std::cout << "VesselGrid Exception: " << e.what() << std::endl;
  }
}

void VesselGrid::PrintLinkage(sep::Axis aAxis) {
  for (int y = 0; y < m_vPrintVectorSize.y(); y++) {
    for (int x = 0; x < m_vPrintVectorSize.x(); x++) {
      Cell* pCell = m_vPrintVector[x][y];
      if (pCell) {
        std::cout << pCell->m_vType[aAxis];
      }
      else {
        std::cout << "x";
      }
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;

  //for (auto& item : m_vCells) {
  //  std::cout << item->m_vType[aAxis];
  //}
  //std::cout << std::endl; 

  //for (int y = 0; y < m_vPrintVectorSize.y(); y++) {
  //  std::cout << GetBorderCell(y)->m_vType[aAxis];
  //}
  //std::cout << std::endl;
}


int VesselGrid::compute_binary_summ(int value) {
  int n = compute_binary_log(value);
  return compute_binary_pow(2, n + 1) - 1;
}

int VesselGrid::compute_binary_pow(int value, int n) {
  int result = 1;
  for (int i = 0; i < n; i++) {
    result *= value;
  }
  return result;
}

int VesselGrid::compute_binary_log(int value) {
  int result = 0;
  while (value % 2 == 0) {
    result++;
    value = value / 2;
  }
  if (value == 1) return result;
  else {
    // Error
    return -1;
  }
}

/* compute part */

void VesselGrid::computeType(sep::Axis aAxis) {
  for (auto& item : m_vCells) {
    item->computeType(aAxis);
  }
}

void VesselGrid::computeHalf(sep::Axis aAxis) {
  for (auto& item : m_vCells) {
    item->computeHalf(aAxis);
  }
}

void VesselGrid::computeValue(sep::Axis aAxis) {
  for (auto& item : m_vCells) {
    item->computeValue(aAxis);
  }
}

void VesselGrid::computeIntegral(unsigned int gi0, unsigned int gi1) {
  for (auto& item : m_vCells) {
    item->computeIntegral(gi0, gi1);
  }
}

void VesselGrid::computeMacroData() {
  for (auto& item : m_vCells) {
    item->computeMacroData();
  }
}

/* LeftVesselGrid */

void LeftVesselGrid::create_normal_vessel() {
  // calc num cells, allocate space .... ?!
  int iNumCells = compute_binary_summ(m_pVGInfo->iNy - 2);
  iNumCells += m_pVGInfo->iAdditionalLenght;

  int iCoreLenght = compute_binary_log(m_pVGInfo->iNy - 2) + 1;
  int iLenght = iCoreLenght + m_pVGInfo->iAdditionalLenght;
  iNumCells += iLenght * 2;

  iLenght += 1; // last one for LeftBorderVesselCell
  iNumCells += 1; // last one for LeftBorderVesselCell

  std::cout << "LeftVesselGrid: iNy = " << m_pVGInfo->iNy << std::endl;
  std::cout << "LeftVesselGrid: iCoreLenght = " << iCoreLenght << std::endl;
  std::cout << "LeftVesselGrid: iLenght = " << iLenght << std::endl;
  std::cout << "LeftVesselGrid: NumCells = " << iNumCells << std::endl;

  m_vCells.clear();

  for (int iNx = 0; iNx < iLenght; iNx++) {
    // Max Ny index
    int iNyMax = 0;
    if (iNx == 0) {
      // Left Border Cell
      iNyMax = 1;
    }
    else if (iNx < m_pVGInfo->iAdditionalLenght + 1) {
      // Additional cells
      iNyMax = 3;
    } else {
      int iNxCore = iNx - m_pVGInfo->iAdditionalLenght - 1;
      iNyMax = compute_binary_pow(2, iNxCore) + 2;
      //std::cout << "iNyMax = " << iNyMax << std::endl;
    }
    for (int iNy = 0; iNy < iNyMax; iNy++) {
      std::shared_ptr<Cell> pCell(new Cell(m_pGridManager));
      m_vCells.push_back(pCell);
      Vector3d vAreastep = m_pVGInfo->vAreastep;
      vAreastep.y() *= (m_pVGInfo->iNy - 2) / (iNyMax - 2);
      pCell->setParameters(m_pVGInfo->dStartConcentration, m_pVGInfo->dStartTemperature, vAreastep);
      pCell->Init();
    }
  }
}

void LeftVesselGrid::create_cycled_vessel() {
  // calc num cells, allocate space .... ?!
  int iNumCells = compute_binary_summ(m_pVGInfo->iNy);
  iNumCells += m_pVGInfo->iAdditionalLenght;

  int iCoreLenght = compute_binary_log(m_pVGInfo->iNy) + 1;
  int iLenght = iCoreLenght + m_pVGInfo->iAdditionalLenght;

  iLenght += 1; // last one for LeftBorderVesselCell
  iNumCells += 1; // last one for LeftBorderVesselCell

  std::cout << "LeftVesselGrid: iNy = " << m_pVGInfo->iNy << std::endl;
  std::cout << "LeftVesselGrid: iCoreLenght = " << iCoreLenght << std::endl;
  std::cout << "LeftVesselGrid: iLenght = " << iLenght << std::endl;
  std::cout << "LeftVesselGrid: NumCells = " << iNumCells << std::endl;

  m_vCells.clear();

  for (int iNx = 0; iNx < iLenght; iNx++) {
    // Max Ny index
    int iNyMax = 0;
    if (iNx == 0) {
      // Left Border Cell
      iNyMax = 1;
    }
    else if (iNx < m_pVGInfo->iAdditionalLenght + 1) {
      // Additional cells
      iNyMax = 1;
    }
    else {
      int iNxCore = iNx - m_pVGInfo->iAdditionalLenght - 1;
      iNyMax = compute_binary_pow(2, iNxCore);
      //std::cout << "iNyMax = " << iNyMax << std::endl;
    }
    for (int iNy = 0; iNy < iNyMax; iNy++) {
      std::shared_ptr<Cell> pCell(new Cell(m_pGridManager));
      m_vCells.push_back(pCell);
      Vector3d vAreastep = m_pVGInfo->vAreastep;
      vAreastep.y() *= (m_pVGInfo->iNy) / (iNyMax);
      pCell->setParameters(m_pVGInfo->dStartConcentration, m_pVGInfo->dStartTemperature, vAreastep);
      pCell->Init();
    }
  }
}

void LeftVesselGrid::link_normal_vessel() {
  int iCoreLenght = compute_binary_log(m_pVGInfo->iNy - 2) + 1;
  int iLenght = iCoreLenght + m_pVGInfo->iAdditionalLenght;

  iLenght += 1; // last one for LeftBorderVesselCell

  int iStartIndex = 0;

  for (int iNx = 0; iNx < iLenght; iNx++) {
    // Max Ny index
    int iNyMax = 0;
    if (iNx == 0) {
      // Left Border Cell
      iNyMax = 1;
      m_vCells[iStartIndex]->m_vNext[sep::X].push_back(m_vCells[iStartIndex + 2].get());
      m_vCells[iStartIndex + 2]->m_vPrev[sep::X].push_back(m_vCells[iStartIndex].get());
    }
    else if (iNx < m_pVGInfo->iAdditionalLenght + 1) {
      // Additional cells
      iNyMax = 3;
      for (int iNy = 0; iNy < iNyMax; iNy++) {
        int iIndex = iStartIndex + iNy;
        Cell* pCell = m_vCells[iIndex].get();

        int iNextX = iIndex + (iNyMax - iNy) + 2 * (iNy - 1) + 1;
        int iNextY = iIndex + 1;
        int iPrevY = iIndex - 1;

        if (iNy == 0) {
          pCell->m_vNext[sep::Y].push_back(m_vCells[iNextY].get());
        }
        else if (iNy = iNyMax - 1) {
          pCell->m_vPrev[sep::Y].push_back(m_vCells[iPrevY].get());
        }
        else {
          if (iNx != iLenght - 1) {
            pCell->m_vNext[sep::X].push_back(m_vCells[iNextX].get());
            m_vCells[iNextX]->m_vPrev[sep::X].push_back(pCell);

            pCell->m_vPrev[sep::Y].push_back(m_vCells[iPrevY].get());
            pCell->m_vNext[sep::Y].push_back(m_vCells[iNextY].get());
          }
        }
      }
    }
    else {
      int iNxCore = iNx - m_pVGInfo->iAdditionalLenght - 1;
      iNyMax = compute_binary_pow(2, iNxCore) + 2;

      for (int iNy = 0; iNy < iNyMax; iNy++) {
        int iIndex = iStartIndex + iNy;
        Cell* pCell = m_vCells[iIndex].get();

        int iNextX = iIndex + (iNyMax - iNy) + 2 * (iNy - 1) + 1;
        int iNextY = iIndex + 1;
        int iPrevY = iIndex - 1;

        if (iNy == 0) {
          pCell->m_vNext[sep::Y].push_back(m_vCells[iNextY].get());
        }
        else if (iNy == iNyMax - 1) {
          pCell->m_vPrev[sep::Y].push_back(m_vCells[iPrevY].get());
        }
        else {
          if (iNx != iLenght - 1) {
            pCell->m_vNext[sep::X].push_back(m_vCells[iNextX].get());
            pCell->m_vNext[sep::X].push_back(m_vCells[iNextX + 1].get());

            m_vCells[iNextX]->m_vPrev[sep::X].push_back(pCell);
            m_vCells[iNextX + 1]->m_vPrev[sep::X].push_back(pCell);
          }

          pCell->m_vPrev[sep::Y].push_back(m_vCells[iPrevY].get());
          pCell->m_vNext[sep::Y].push_back(m_vCells[iNextY].get());
        }
      }
    }
    iStartIndex += iNyMax;
  }
}

void LeftVesselGrid::link_cycled_vessel() {
  int iCoreLenght = compute_binary_log(m_pVGInfo->iNy) + 1;
  int iLenght = iCoreLenght + m_pVGInfo->iAdditionalLenght;

  iLenght += 1; // last one for LeftBorderVesselCell

  int iStartIndex = 0;

  for (int iNx = 0; iNx < iLenght; iNx++) {
    // Max Ny index
    int iNyMax = 0;
    if (iNx == 0) {
      // Left Border Cell
      iNyMax = 1;
      m_vCells[iStartIndex]->m_vNext[sep::X].push_back(m_vCells[iStartIndex + 1].get());
      m_vCells[iStartIndex + 1]->m_vPrev[sep::X].push_back(m_vCells[iStartIndex].get());
    }
    else if (iNx < m_pVGInfo->iAdditionalLenght + 1) {
      // Additional cells
      iNyMax = 1;
      if (iNx != iLenght - 1) {
        m_vCells[iStartIndex]->m_vNext[sep::X].push_back(m_vCells[iStartIndex + 1].get());
        m_vCells[iStartIndex + 1]->m_vPrev[sep::X].push_back(m_vCells[iStartIndex].get());

        m_vCells[iStartIndex]->m_vNext[sep::Y].push_back(m_vCells[iStartIndex].get());
        m_vCells[iStartIndex]->m_vPrev[sep::Y].push_back(m_vCells[iStartIndex].get());
      }
    }
    else {
      int iNxCore = iNx - m_pVGInfo->iAdditionalLenght - 1;
      iNyMax = compute_binary_pow(2, iNxCore);

      for (int iNy = 0; iNy < iNyMax; iNy++) {
        int iIndex = iStartIndex + iNy;
        Cell* pCell = m_vCells[iIndex].get();

        int iNextX = iIndex + (iNyMax - iNy) + 2 * (iNy);
        int iNextY = iIndex + 1;
        int iPrevY = iIndex - 1;

        if (iNy == 0) iPrevY = iStartIndex + iNyMax - 1;
        if (iNy == iNyMax - 1) iNextY = iStartIndex;

        if (iNx != iLenght - 1) {
          pCell->m_vNext[sep::X].push_back(m_vCells[iNextX].get());
          pCell->m_vNext[sep::X].push_back(m_vCells[iNextX + 1].get());

          m_vCells[iNextX]->m_vPrev[sep::X].push_back(pCell);
          m_vCells[iNextX + 1]->m_vPrev[sep::X].push_back(pCell);
        }

        pCell->m_vPrev[sep::Y].push_back(m_vCells[iPrevY].get());
        pCell->m_vNext[sep::Y].push_back(m_vCells[iNextY].get());
      }
    }
    iStartIndex += iNyMax;
  }
}

void LeftVesselGrid::generate_normal_print_vector() {
  int iCoreLenght = compute_binary_log(m_pVGInfo->iNy - 2) + 1;
  int iLenght = iCoreLenght + m_pVGInfo->iAdditionalLenght;

  iLenght += 1; // last one for LeftBorderVesselCell

  m_vPrintVectorSize.set(iLenght, m_pVGInfo->iNy);

  m_vPrintVector.clear();
  m_vPrintVector.resize(m_vPrintVectorSize.x());
  for (auto& item : m_vPrintVector) {
    item.resize(m_vPrintVectorSize.y(), nullptr);
  }

  int iIndexStart = 0;
  for (int iPrintNx = 0; iPrintNx < m_vPrintVectorSize.x(); iPrintNx++) {
    // Max Ny index
    int iNyMax = 0;
    if (iPrintNx == 0) {
      // Left Border Cell
      iNyMax = 1;
    }
    else if (iPrintNx < m_pVGInfo->iAdditionalLenght + 1) {
      // Additional cells
      iNyMax = 3;
    }
    else {
      int iNxCore = iPrintNx - m_pVGInfo->iAdditionalLenght - 1;
      iNyMax = compute_binary_pow(2, iNxCore) + 2;
      //std::cout << "iNyMax = " << iNyMax << std::endl;
    }
    for (int iPrintNy = 0; iPrintNy < m_vPrintVectorSize.y(); iPrintNy++) {
      int iIndex = iIndexStart;
      if (iNyMax == 1) {
        if (iPrintNy == 0) {
          iIndex = -1;
        }
        else if (iPrintNy == m_vPrintVectorSize.y() - 1) {
          iIndex = -1;
        }
        else {
          iIndex += 0;
        }
      }
      else {
        if (iPrintNy == 0) {
          iIndex += 0;
        }
        else if (iPrintNy == m_vPrintVectorSize.y() - 1) {
          iIndex += iNyMax - 1;
        }
        else {
          iIndex += ((iNyMax - 2) * (iPrintNy - 1) / (m_vPrintVectorSize.y() - 2)) + 1;
        }
      }
      //std::cout << iIndex << " : ";
      if (iIndex < 0) {
        m_vPrintVector[iPrintNx][iPrintNy] = nullptr;
      }
      else {
        m_vPrintVector[iPrintNx][iPrintNy] = m_vCells[iIndex].get();
      }
    }
    iIndexStart += iNyMax;
    //std::cout << std::endl;
  }
}

void LeftVesselGrid::generate_cycled_print_vector() {
  int iCoreLenght = compute_binary_log(m_pVGInfo->iNy) + 1;
  int iLenght = iCoreLenght + m_pVGInfo->iAdditionalLenght;

  iLenght += 1; // last one for LeftBorderVesselCell

  m_vPrintVectorSize.set(iLenght, m_pVGInfo->iNy);

  m_vPrintVector.clear();
  m_vPrintVector.resize(m_vPrintVectorSize.x());
  for (auto& item : m_vPrintVector) {
    item.resize(m_vPrintVectorSize.y(), nullptr);
  }

  int iIndexStart = 0;
  for (int iPrintNx = 0; iPrintNx < m_vPrintVectorSize.x(); iPrintNx++) {
    // Max Ny index
    int iNyMax = 0;
    if (iPrintNx == 0) {
      // Left Border Cell
      iNyMax = 1;
    }
    else if (iPrintNx < m_pVGInfo->iAdditionalLenght + 1) {
      // Additional cells
      iNyMax = 1;
    }
    else {
      int iNxCore = iPrintNx - m_pVGInfo->iAdditionalLenght - 1;
      iNyMax = compute_binary_pow(2, iNxCore);
      //std::cout << "iNyMax = " << iNyMax << std::endl;
    }
    for (int iPrintNy = 0; iPrintNy < m_vPrintVectorSize.y(); iPrintNy++) {
      int iIndex = iIndexStart;
      if (iNyMax == 1) {
        if (iPrintNy == 0) {
          iIndex = -1;
        }
        else if (iPrintNy == m_vPrintVectorSize.y() - 1) {
          iIndex = -1;
        }
        else {
          iIndex += 0;
        }
      }
      else {
        if (iPrintNy == 0) {
          iIndex += 0;
        }
        else if (iPrintNy == m_vPrintVectorSize.y() - 1) {
          iIndex += iNyMax - 1;
        }
        else {
          iIndex += ((iNyMax) * (iPrintNy) / (m_vPrintVectorSize.y()));
        }
      }
      //std::cout << iIndex << " : ";
      if (iIndex < 0) {
        m_vPrintVector[iPrintNx][iPrintNy] = nullptr;
      }
      else {
        m_vPrintVector[iPrintNx][iPrintNy] = m_vCells[iIndex].get();
      }
    }
    iIndexStart += iNyMax;
    //std::cout << std::endl;
  }
}

/* RightVesselGrid */

void RightVesselGrid::create_normal_vessel() {

}

void RightVesselGrid::create_cycled_vessel() {

}

void RightVesselGrid::link_normal_vessel() {

}

void RightVesselGrid::link_cycled_vessel() {

}

void RightVesselGrid::generate_normal_print_vector() {

}

void RightVesselGrid::generate_cycled_print_vector() {

}