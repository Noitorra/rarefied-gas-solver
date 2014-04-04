#pragma once
#include "main.h"

// this is actually Dima's class
class Solver {
public:
  Solver();
private:
  std::shared_ptr<class Grid> m_pGrid;
};