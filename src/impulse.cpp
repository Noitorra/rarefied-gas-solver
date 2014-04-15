#include "impulse.h"

#include "grid_manager.h"
#include "solver.h"
#include "gas.h"

#include <iostream>
#include <algorithm>

Impulse::Impulse() :
m_pGridManager(nullptr),
m_pGrid(nullptr),
m_pSolver(nullptr),
m_pConfig(nullptr) {
	m_dMaxImpulse = 4.8;
	m_uResolution = 20;

	m_dDeltaImpulse = 0.0;
	m_dDeltaImpulseQube = 0.0;

  m_pxyz2i = nullptr;
}

Impulse::~Impulse() {
  if (m_pxyz2i) {
    for (unsigned int x = 0; x < m_uResolution; x++) {
      for (unsigned int y = 0; y < m_uResolution; y++) {
        delete[] m_pxyz2i[x][y];
      }
      delete[] m_pxyz2i[x];
    }
    delete[] m_pxyz2i;
  }
}

void Impulse::Init(GridManager* pGridManager) {
  m_pGridManager = pGridManager;
  m_pGrid = pGridManager->GetGrid();
  m_pSolver = pGridManager->GetSolver();
  m_pConfig = pGridManager->GetConfig();
  
  // TODO: do something with that...
  GasVector& gasv = m_pSolver->GetGas();
  if (gasv.size() >= 2) {
    m_dMaxImpulse = std::max(gasv[0]->getMass(), gasv[1]->getMass()) * m_dMaxImpulse;
    std::cout << "Impulse::Init() : gasv.size() >= 2" << std::endl;
  }
  else {
    std::cout << "Impulse::Init() : gasv.size() < 2" << std::endl;
  }
  std::cout << "Impulse::Init() : m_dMaxImpulse = " << m_dMaxImpulse << std::endl;

  // calc delta impulse
  m_dDeltaImpulse = 2*m_dMaxImpulse/(m_uResolution);
  m_dDeltaImpulseQube = std::pow(m_dDeltaImpulse, 3);

  // calc line impulse vector
  std::vector<double> line;
  for( unsigned int i=0; i<m_uResolution; i++ ) {
    line.push_back( m_dDeltaImpulse*(i+0.5) - m_dMaxImpulse );
    // std::cout << "[Impulse] impulse[" << i << "] = " << line.back() << std::endl;
  }

  // xyz2i
  m_pxyz2i = new int**[m_uResolution];

  // creating impulse sphere
  for(unsigned int x=0;x<line.size();x++) {
    m_pxyz2i[x] = new int*[m_uResolution];
    for(unsigned int y=0;y<line.size();y++) {
      m_pxyz2i[x][y] = new int[m_uResolution];
      for(unsigned int z=0;z<line.size();z++) {
        Vector3d vec(line[x], line[y], line[z]);
        if (vec.mod() < m_dMaxImpulse) {
          m_pxyz2i[x][y][z] = (int)m_vImpulse.size();
          m_vImpulse.push_back(vec);
        } else {
          // TODO: the fuck should i do here ... ?
          m_pxyz2i[x][y][z] = -1;
        }
      }
    }
  }
}

void Impulse::setMaxImpulse(double max_impulse) {
	m_dMaxImpulse = max_impulse;
}
void Impulse::setResolution(unsigned int resolution) {
	m_uResolution = resolution;
}

unsigned int Impulse::getResolution() {
	return m_uResolution;
}

double Impulse::getMaxImpulse() {
	return m_dMaxImpulse;
}

double Impulse::getDeltaImpulse() {
	return m_dDeltaImpulse;
}

double Impulse::getDeltaImpulseQube() {
	return m_dDeltaImpulseQube;
}

std::vector< Vector3d >& Impulse::getVector() {
	return m_vImpulse;
}
