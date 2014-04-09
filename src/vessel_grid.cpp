#include "vessel_grid.h"
#include "cell.h"

void VesselGrid::createVessel(VesselGridType eType) {
  switch (eType) {
  case VGT_NORMAL:
    create_normal_vessel();
    break;
  case VGT_CYCLED:
    create_cycled_vessel();
    break;
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
      Cell* newCell = new Cell(/* ... */nullptr);
      // newCell->setParameters(m_pVGInfo->dStartConcentration, m_pVGInfo->dStartTemperature, m_pVGInfo->vAreastep);
    }

    if (iAdditionalLenght < m_pVGInfo->iAdditionalLenght) {
      iAdditionalLenght++;
    } else {
      iNy *= 2;
    }
  }
}
