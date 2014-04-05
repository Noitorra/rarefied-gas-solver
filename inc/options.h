/*
 * options.h
 *
 *  Created on: 04 апр. 2014 г.
 *      Author: kisame
 */

#ifndef OPTIONS_H_
#define OPTIONS_H_

#include "main.h"

class Config;

class Options {
public:
	Options();
	virtual ~Options();
  
  void Init();
  // Throws char const*
  Config* GetActiveConfig();
  
private:
  void AddConfig(std::shared_ptr<Config> pConfig);
  bool SetActiveConfig(const std::string& name);
  
  // Name + config
  std::map<std::string, std::shared_ptr<Config>> m_mConfigs;
  std::string m_sActiveConfig;
};

#endif /* OPTIONS_H_ */
