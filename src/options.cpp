/*
 * options.cpp
 *
 *  Created on: 04 апр. 2014 г.
 *      Author: kisame
 */

#include "options.h"
#include "config.h"

Options::Options() {
	// TODO Auto-generated constructor stub
	m_cSolverInfo = nullptr;
}

Options::~Options() {
	// TODO Auto-generated destructor stub
}

void Options::setSolverInfo(SolverInfo* _SolverInfo) {
	m_cSolverInfo = _SolverInfo;
}

SolverInfo* Options::getSolverInfo() {
	return m_cSolverInfo;
}

void Options::Init() {
  // Feel free to add your own configuration
  // All the missed parameters will be filled with default
  
  // Debug configuration 1
  std::shared_ptr<Config> pDebugConfig1(new Config("debug_1"));
  pDebugConfig1->SetGridSize(Vector3i(16, 16, 1));
  pDebugConfig1->SetOutputGridStart(Vector3i(16, 0, 0));
  pDebugConfig1->SetOutputSize(Vector3i(32, 16, 1));
  pDebugConfig1->SetGridGeometryType(sep::DEBUG1_GRID_GEOMETRY);
  pDebugConfig1->SetMaxIteration(5);
  pDebugConfig1->SetUseIntegral(true);
  pDebugConfig1->SetUseVessels(true);
  pDebugConfig1->SetUseLooping(true);
  AddConfig(pDebugConfig1);
  
  // Bigger grid
  std::shared_ptr<Config> pDebugConfig2(new Config("more_bigger"));
  pDebugConfig2->SetGridSize(Vector3i(60, 18, 1));
  pDebugConfig2->SetOutputGridStart(Vector3i(26, 0, 0));
  pDebugConfig2->SetOutputSize(Vector3i(86, 18, 1));
  pDebugConfig2->SetGridGeometryType(sep::DIMAN_GRID_GEOMETRY);
  pDebugConfig2->SetMaxIteration(200);
  pDebugConfig2->SetUseIntegral(true);
  pDebugConfig2->SetUseVessels(true);
  pDebugConfig2->SetUseLooping(false);
  AddConfig(pDebugConfig2);
  
  // Bigger grid 32x16
  std::shared_ptr<Config> pDebugConfig3(new Config("debug_3"));
  pDebugConfig3->SetGridSize(Vector3i(18, 18, 1));
  pDebugConfig3->SetOutputGridStart(Vector3i(6, 0, 0));
  pDebugConfig3->SetOutputSize(Vector3i(24, 18, 1));
  pDebugConfig3->SetGridGeometryType(sep::DIMAN_GRID_GEOMETRY);
  pDebugConfig3->SetMaxIteration(100);
  pDebugConfig3->SetUseIntegral(true);
  pDebugConfig3->SetUseVessels(true);
  pDebugConfig3->SetUseLooping(false);
  AddConfig(pDebugConfig3);
  
  // H type grid
  std::shared_ptr<Config> pHTypeConfig(new Config("h_type"));
  pHTypeConfig->SetGridSize(Vector3i(40, 32, 1));
  pHTypeConfig->SetGridGeometryType(sep::PROHOR_GRID_GEOMTRY);
  pHTypeConfig->SetMaxIteration(100);
  pHTypeConfig->SetUseIntegral(true);
  pHTypeConfig->SetUseVessels(true);
  pHTypeConfig->SetUseLooping(false);
  AddConfig(pHTypeConfig);
  
  // Finally, select wich is active
//  SetActiveConfig("debug_1");
  SetActiveConfig("more_bigger");
//  SetActiveConfig("debug_3");
//  SetActiveConfig("h_type");
}

void Options::AddConfig(std::shared_ptr<Config> pConfig) {
  m_mConfigs[pConfig->GetName()] = pConfig;
}

bool Options::SetActiveConfig(const std::string& sName) {
  if (m_mConfigs.find(sName) == m_mConfigs.end())
    return false;
  m_sActiveConfig = sName;
  try {
    std::cout << "Activating config: ";
    GetConfig()->PrintMe();
  } catch (char const* sExc) {
    std::cout << "Exception occurs: " << sExc << std::endl;
    return false;
  }
  return true;
}

Config* Options::GetConfig() {
  if (m_mConfigs.find(m_sActiveConfig) == m_mConfigs.end())
    throw "No active config yet";
  return m_mConfigs[m_sActiveConfig].get();
}
