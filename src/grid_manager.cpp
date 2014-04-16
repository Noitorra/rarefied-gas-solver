#include "grid_manager.h"
#include "grid.h"
#include "solver.h"
#include "options.h"
#include "config.h"

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

void GridManager::PrintGrid() {
  const Vector3i& vGridSize = m_pConfig->GetGridSize();
  for (int z = 0; z < vGridSize.z(); z++) {
    for (int y = 0; y < vGridSize.y(); y++) {
      for (int x = 0; x < vGridSize.x(); x++) {
        std::cout << m_pGrid->m_vInitCells[x][y][z]->m_eType << " ";
      }
      std::cout << std::endl;
    }
  }
  std::cout << std::endl;
}

void GridManager::ConfigureGrid() {
  PreprocessGrid();
  ConfigureGridGeometry();
  PrintGrid();
  PostprocessGrid();
  PrintGrid();
}

void GridManager::PreprocessGrid() {
  // TODO: should we do here smth?
}

void GridManager::PostprocessGrid() {
  const Vector3i& vGridSize = m_pConfig->GetGridSize();
  for (int x = 0; x < vGridSize.x(); x++) {
    for (int y = 0; y < vGridSize.y(); y++) {
      for (int z = 0; z < vGridSize.z(); z++) {
        Vector2i v_p(x, y);
        if (m_pGrid->GetInitCell(v_p)->m_eType != sep::FAKE_CELL)
          continue;
        
        sep::NeighbType e_neighb;
        sep::Axis e_ax;
        int i_q;
        // Remove alone fake cells (which without normal neighbour)
        FindNeighbour(v_p, sep::NORMAL_CELL, e_neighb, e_ax, i_q);
        if (!i_q)
          m_pGrid->GetInitCell(v_p)->m_eType = sep::EMPTY_CELL;
      }
    }
  }
}

// Find the position of first neighbour with given type
// and whole quantity of such neighbours
void GridManager::FindNeighbour(Vector2i vP, sep::CellType eType,
                   sep::NeighbType& eNeighb,
                   sep::Axis& eAxis, int& iQuant) {
  int i_quant = 0;
  sep::CellType e_type;
  for (int ax = 0; ax <= sep::Z; ax++) {
    for (int neighb = 0; neighb <= sep::NEXT; neighb++) {
      if (GetNeighbour(vP, (sep::NeighbType)neighb, (sep::Axis)ax, e_type)) {
        if (e_type == eType) {
          if (!i_quant) {
            eNeighb = (sep::NeighbType)neighb;
            eAxis = (sep::Axis)ax;
          }
          i_quant++;
        }
      }
    }
  }
  iQuant = i_quant;
}

bool GridManager::GetNeighbour(Vector2i vP, sep::NeighbType eNeighb,
                  sep::Axis eAxis, sep::CellType& eType) {
  Vector2i v_slash;
  v_slash[eAxis] = GetSlash(eNeighb);
  vP += v_slash;
  sep::CellType e_type;
  try {
    e_type = m_pGrid->GetInitCell(vP)->m_eType;
  } catch(const char* cStr) {
    return false;
  }
  eType = e_type;
  return true;
}

int GridManager::GetSlash(sep::NeighbType eType) const {
  switch (eType) {
    case sep::PREV:
      return -1;
    case sep::NEXT:
      return +1;
      default:
      return 0;
  }
}

void GridManager::SetBox(Vector2i vP, Vector2i vSize) {
  for (int x = 0; x < vSize.x(); x++) {
    for (int y = 0; y < vSize.y(); y++) {
      Vector2i vTempPos = vP + Vector2i(x, y);
      m_pGrid->GetInitCell(vTempPos)->m_eType = sep::NORMAL_CELL;
    }
  }
}

void GridManager::PushTemperature(double dT) {
  m_vTemperatureStack.push_back(dT);
}
double GridManager::PopTemperature() {
  if (m_vTemperatureStack.size() < 1)
    return 0.0;
  double dT = m_vTemperatureStack.back();
  m_vTemperatureStack.pop_back();
  return dT;
}

void GridManager::PushConcentration(double dConc) {
  m_vConcetrationStack.push_back(dConc);
}
double GridManager::PopConcentration() {
  if (m_vConcetrationStack.size() < 1)
    return 0.0;
  double dConc = m_vConcetrationStack.back();
  m_vConcetrationStack.pop_back();
  return dConc;
}