#include "config.h"

using namespace std;

void Config::SetGridSize(const Vector3i& vGridSize) {
  m_vGridSize = vGridSize;
}

void Config::SetOutputSize(const Vector3i& vOutputSize) {
  m_vOutputSize = vOutputSize;
}

void Config::SetOutputGridStart(const Vector3i& vOutputGridStart) {
  m_vOutputGridStart = vOutputGridStart;
}

void Config::PrintMe() {
  cout << "Config " << m_sName << endl;
  cout << "GridSize = " << m_vGridSize.x() <<
  "x" << m_vGridSize.y() <<
  "x" << m_vGridSize.z() << endl;
  cout << "OutputSize = " << m_vOutputSize.x() <<
  "x" << m_vOutputSize.y() <<
  "x" << m_vOutputSize.z() << endl;
}

void Config::SetGridGeometryType(sep::GridGeometry eGridGeometryType) {
  m_eGridGeometryType = eGridGeometryType;
};