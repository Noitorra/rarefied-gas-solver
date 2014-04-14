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
  pDebugConfig2->SetGridSize(Vector3i(512, 64, 1));
  pDebugConfig2->SetOutputGridStart(Vector3i(0, 0, 0));
  pDebugConfig2->SetOutputSize(Vector3i(512, 64, 1));
  pDebugConfig2->SetGridGeometryType(sep::DIMAN_GRID_GEOMETRY);
  pDebugConfig2->SetMaxIteration(100);
  pDebugConfig2->SetUseIntegral(false);
  pDebugConfig2->SetUseVessels(false);
  pDebugConfig2->SetUseLooping(true);
  AddConfig(pDebugConfig2);
  
  // Bigger grid 32x16
  std::shared_ptr<Config> pDebugConfig3(new Config("debug_3"));
  pDebugConfig3->SetGridSize(Vector3i(4, 4, 1));
  pDebugConfig3->SetOutputGridStart(Vector3i(0, 0, 0));
  pDebugConfig3->SetOutputSize(Vector3i(8, 4, 1));
  pDebugConfig3->SetGridGeometryType(sep::DIMAN_GRID_GEOMETRY);
  pDebugConfig3->SetMaxIteration(5);
  pDebugConfig3->SetUseIntegral(true);
  pDebugConfig3->SetUseVessels(true);
  pDebugConfig3->SetUseLooping(false);
  AddConfig(pDebugConfig3);
  
  // H type grid
  std::shared_ptr<Config> pHTypeConfig(new Config("h_type"));
  pHTypeConfig->SetGridSize(Vector3i(130, 70, 1));
  pHTypeConfig->SetOutputGridStart(Vector3i(0, 0, 0));
  pHTypeConfig->SetOutputSize(Vector3i(130, 70, 1));
  pHTypeConfig->SetGridGeometryType(sep::PROHOR_GRID_GEOMTRY);
  pHTypeConfig->SetMaxIteration(1000000);
  pHTypeConfig->SetUseIntegral(true);
  pHTypeConfig->SetUseVessels(false);
  pHTypeConfig->SetUseLooping(false);
  pHTypeConfig->SetAdditionalVesselLenght(0);
  pHTypeConfig->SetUseInitialConditions(true);
  pHTypeConfig->SetResetSomeCellsEachIter(true);
  pHTypeConfig->SetOutputPrefix("D:/GoogleDisk/Shared_Prohor/");
//  pHTypeConfig->SetOutputPrefix("/Volumes/BACKUP HD/Google Drive/Results/");
                                        // + "out/gas0/pressure"
                                        // + "out/gas0/conc"
                                        // + "out/gas0/temp"
  AddConfig(pHTypeConfig);
  
  // Comb type grid
  std::shared_ptr<Config> pCombTypeConfig(new Config("comb_type"));
//  pCombTypeConfig->SetGridSize(Vector3i(512, 64, 1));
//  pCombTypeConfig->SetOutputGridStart(Vector3i(-192, 0, 0));
//  pCombTypeConfig->SetOutputSize(Vector3i(128, 64, 1));
  pCombTypeConfig->SetGridSize(Vector3i(256, 64, 1));
  pCombTypeConfig->SetOutputGridStart(Vector3i(-64, 0, 0));
  pCombTypeConfig->SetOutputSize(Vector3i(128, 64, 1));
  pCombTypeConfig->SetGridGeometryType(sep::DIMAN_GRID_GEOMETRY);
  pCombTypeConfig->SetMaxIteration(2000);
  pCombTypeConfig->SetUseIntegral(true);
  pCombTypeConfig->SetUseVessels(false);
  pCombTypeConfig->SetUseLooping(true);
  pCombTypeConfig->SetAdditionalVesselLenght(0);
  AddConfig(pCombTypeConfig);
  
  // Finally, select wich is active
  SetActiveConfig("debug_3");
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
