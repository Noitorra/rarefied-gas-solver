#include "config.h"

using namespace std;

void Config::SetGridSize(const std::vector<int>& vGridSize) {
  m_vGridSize = vGridSize;
}

void Config::PrintMe() {
  cout << "Config " << m_sName << ": " << endl <<
  "size = " << m_vGridSize[sep::X] << "x" << m_vGridSize[sep::Y] <<
  "x" << m_vGridSize[sep::Z] << endl <<
  "grid_geom_type = " << m_eGridGeometryType << endl;
}

void Config::SetGridGeometryType(sep::GridGeometry eGridGeometryType) {
  m_eGridGeometryType = eGridGeometryType;
};