#include "vessel_grid.h"
#include "cell.h"

void VesselGrid::CreateAndLinkVessel() {
  switch (m_eType) {
  case VGT_NORMAL:
    create_normal_vessel();
    link_normal_vessel();
    generate_cycled_print_vector(m_vPrintVector);
    break;
  case VGT_CYCLED:
    create_cycled_vessel();
    link_cycled_vessel();
    generate_cycled_print_vector(m_vPrintVector);
    break;
  }
}

void VesselGrid::PrintLinkage(sep::Axis aAxis) {
  for (unsigned int i = 0; i < m_vPrintVector.size(); i++) {
    std::cout << m_vPrintVector[i]->m_vType[aAxis];
    if (i % m_pVGInfo->iNy) std::cout << std::endl;
  }
  std::cout << std::endl;
}


int VesselGrid::compute_binary_summ(int value) {
  if (value % 2 != 0) {
    std::cout << "VesselGrid::compute_binary_summ() : value is not binary - " << value << std::endl;
    return 0;
  }
  int result = 0;
  do {
    value = value / 2;
    result += value;
  } while (value != 1);
  return result;
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

}

void LeftVesselGrid::create_cycled_vessel() {
  int iNy = 1;
  int iAdditionalLenght = 0;
  while (iNy <= m_pVGInfo->iNy) {
    // Here we create line of iNy cells
    for (int i = 0; i < iNy; i++) {
      // TODO: Make refactoring ...
      std::shared_ptr<Cell> pCell( new Cell(/* ... */) );
      Vector3d vAreastep = m_pVGInfo->vAreastep;
      vAreastep.y() *= m_pVGInfo->iNy / iNy;
      pCell->setParameters(m_pVGInfo->dStartConcentration, m_pVGInfo->dStartTemperature, vAreastep);
      //pCell->Init();
      m_vCells.push_back(pCell);
    }

    if (iAdditionalLenght < m_pVGInfo->iAdditionalLenght) {
      iAdditionalLenght++;
    } else {
      iNy *= 2;
    }
  }
}

void LeftVesselGrid::link_normal_vessel() {

}

void LeftVesselGrid::link_cycled_vessel() {
  int iNy = 1;
  for (int i = 0; i < (m_vCells.size() - m_pVGInfo->iNy); i++) {
    // line part
    if (i < m_pVGInfo->iAdditionalLenght) {
      m_vCells[i]->m_vNext[sep::X].push_back(m_vCells[i + 1].get());
      m_vCells[i + 1]->m_vPrev[sep::X].push_back(m_vCells[i].get());

      m_vCells[i]->m_vNext[sep::Y].push_back(m_vCells[i].get());
      m_vCells[i]->m_vPrev[sep::Y].push_back(m_vCells[i].get());
    } else {
      int iIndexY = (i - m_pVGInfo->iAdditionalLenght) - compute_binary_summ(iNy);
      // new iIndexY is index of cell in Y line
      int iShiftY = iNy - iIndexY + iIndexY * 2;

      //std::cout << "iIndexY = " << iIndexY 
      //  << " iShiftY = " << iShiftY << std::endl;


      m_vCells[i]->m_vNext[sep::X].push_back(m_vCells[i + iShiftY].get());
      m_vCells[i]->m_vNext[sep::X].push_back(m_vCells[i + iShiftY + 1].get());
      m_vCells[i + iShiftY]->m_vPrev[sep::X].push_back(m_vCells[i].get());
      m_vCells[i + iShiftY + 1]->m_vPrev[sep::X].push_back(m_vCells[i].get());

      int iNextY = i + 1;
      int iPrevY = i - 1;

      if (iIndexY == 0) iPrevY = i + (iNy - 1);
      if (iIndexY == iNy - 1) iNextY = i - (iNy - 1);

      m_vCells[i]->m_vPrev[sep::Y].push_back(m_vCells[iPrevY].get());
      m_vCells[i]->m_vNext[sep::Y].push_back(m_vCells[iNextY].get());

      if (iIndexY == iNy - 1) iNy *= 2;
    }
  }
}

void LeftVesselGrid::generate_normal_print_vector(std::vector<Cell*>& vPrintVector) {

}

void LeftVesselGrid::generate_cycled_print_vector(std::vector<Cell*>& vPrintVector) {
  for (int iNy = 0; iNy < m_pVGInfo->iNy; iNy++) {
    for (int i = 1; i <= m_pVGInfo->iNy; i *= 2) {
      int iStartValue = i - 1;
      int iIndex = iStartValue + iNy / (m_pVGInfo->iNy / i);
      // std::cout << iIndex << " : ";
      vPrintVector.push_back(m_vCells[iIndex].get());
    }
    // std::cout << std::endl;
  }
  // std::cout << std::endl;
}
