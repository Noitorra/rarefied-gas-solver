#include "options.h"
#include "config.h"
#include "grid_manager.h"

Options::Options() {}

void Options::Init(GridManager* pGridManager) {
  // Feel free to add your own configuration
  // All the missed parameters will be filled with default
  
  // Debug configuration
  std::shared_ptr<Config> pDebugConfig(new Config("debug"));
  pDebugConfig->SetGridSize(Vector3i(10, 10, 1));
  pDebugConfig->SetOutputGridStart(Vector3i(0, 0, 0));
  pDebugConfig->SetOutputSize(Vector3i(10, 10, 1));
  pDebugConfig->SetGridGeometryType(sep::COMB_GRID_GEOMETRY);
  pDebugConfig->SetMaxIteration(0);
  pDebugConfig->SetUseIntegral(true);
  AddConfig(pDebugConfig);
  
  SetActiveConfig("debug");
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
