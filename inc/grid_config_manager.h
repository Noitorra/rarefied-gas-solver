#pragma once
#include "main.h"

class GridConfigManader {
public:
  bool Write(const std::string& name);
  bool Read(const std::string& name);
};