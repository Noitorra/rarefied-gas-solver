#pragma once
#include "main.h"

// this is actually Dima's class
class Solver {
public:
  Solver();
  void Init();
  
private:
  class Grid* m_pSolver;
  class Grid* m_pGrid;
};