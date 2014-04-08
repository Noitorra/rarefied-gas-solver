#include "config.h"

using namespace std;

void Config::SetGridSize(const Vector3i& vGridSize) {
  // 2D case only
  m_vGridSize = Vector3i(vGridSize.x() + 2, vGridSize.y() + 2, vGridSize.z());
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