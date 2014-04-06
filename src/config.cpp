#include "config.h"

using namespace std;

void Config::SetGridSize(const Vector3i& vGridSize) {
  m_vGridSize = vGridSize;
}

void Config::PrintMe() {
  cout << "Config " << m_sName << ": " << endl <<
  "size = " << m_vGridSize.x() << "x" << m_vGridSize.y() <<
  "x" << m_vGridSize.z() << endl <<
  "grid_geom_type = " << m_eGridGeometryType << endl;
}

void Config::SetGridGeometryType(sep::GridGeometry eGridGeometryType) {
  m_eGridGeometryType = eGridGeometryType;
};