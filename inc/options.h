#ifndef OPTIONS_H_
#define OPTIONS_H_

#include "main.h"

class SolverInfo;
class Config;
class GridManager;

class Options {
public:
	Options();
  
  void Init(GridManager* pGridManager);
  // Throws char const*
  Config* GetConfig(); // Get active config
  
private:
  void AddConfig(std::shared_ptr<Config> pConfig);
  bool SetActiveConfig(const std::string& name);
  
  // Name + config
  std::map<std::string, std::shared_ptr<Config>> m_mConfigs;
  std::string m_sActiveConfig;
};

#endif /* OPTIONS_H_ */
