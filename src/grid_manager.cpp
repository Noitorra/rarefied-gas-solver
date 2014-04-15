#include "grid_manager.h"
#include "grid.h"
#include "solver.h"
#include "options.h"

GridManager::GridManager() :
m_pGrid(new Grid),
m_pSolver(new Solver),
m_pOptions(new Options),
m_pConfig(nullptr) {
}

void GridManager::Init() {
  m_pOptions->Init(this);
  m_pConfig = m_pOptions->GetConfig();
  m_pGrid->Init(this);
  m_pSolver->Init(this);
}

void GridManager::ConfigureGrid() {
  
}

void GridManager::SetBox() {
  
}

void GridManager::PushTemperature(double dT) {
  m_TemperatureStack.push_back(dT);
}

double GridManager::PopTemperature() {
  if (m_TemperatureStack.size() < 1)
    return 0.0;
  double dT = m_TemperatureStack.back();
  m_TemperatureStack.pop_back();
  return dT;
}