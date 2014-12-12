#include "grid_manager.h"
#include "grid.h"
#include "solver.h"
#include "config.h"
#include "cell.h"

GridManager::GridManager() :
m_pGrid(new Grid),
m_pSolver(new Solver) {}

void GridManager::Init() {
  m_pGrid->Init(this);
  m_pSolver->Init(this);
}

void GridManager::PrintGrid() {
  const Vector3i& vGridSize = Config::vGridSize;
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

void GridManager::PrintLinkage(sep::Axis eAxis) {
  const Vector3i& vGridSize = Config::vGridSize;
  for (int z = 0; z < vGridSize.z(); z++) {
    for (int y = 0; y < vGridSize.y(); y++) {
      for (int x = 0; x < vGridSize.x(); x++) {
        Vector2i v_p(x, y);
        if (m_pGrid->GetInitCell(v_p)->m_eType == sep::EMPTY_CELL) {
          std::cout << "x ";
          continue;
        }
        std::cout << m_pGrid->GetInitCell(v_p)->m_pCell->m_vType[eAxis] << " ";
      }
      std::cout << std::endl;
    }
  }
  std::cout << std::endl;
}

void GridManager::ConfigureGrid() {
  PreprocessGrid();
  ConfigureGridGeometry();
  PostprocessGrid();
  PrintGrid();
  
  FillInGrid();
  LinkCells();
  InitCells();
}

void GridManager::PreprocessGrid() {
  // TODO: should we do here smth?
}

void GridManager::PostprocessGrid() {
  const Vector3i& vGridSize = Config::vGridSize;
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
        FindNeighbour(v_p, sep::NORMAL_CELL, e_ax, e_neighb, i_q);
        if (!i_q)
          m_pGrid->GetInitCell(v_p)->m_eType = sep::EMPTY_CELL;
      }
    }
  }
}

// Find the position of first neighbour with given type
// and whole quantity of such neighbours
void GridManager::FindNeighbour(Vector2i vP, sep::CellType eType,
                   sep::Axis& eAxis,
                                sep::NeighbType& eNeighb,
                   int& iQuant) {
  int i_quant = 0;
  sep::CellType e_type;
  for (int ax = 0; ax <= sep::Z; ax++) {
    for (int neighb = 0; neighb <= sep::NEXT; neighb++) {
      if (GetNeighbour(vP, (sep::Axis)ax, (sep::NeighbType)neighb, e_type)) {
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

bool GridManager::FindNeighbourWithIndex(Vector2i vP, sep::CellType eType,
                                int iIndex, sep::Axis& eAxis,
                                sep::NeighbType& eNeighb) {
  int i_quant = 0;
  sep::CellType e_type;
  for (int ax = 0; ax <= sep::Z; ax++) {
    for (int neighb = 0; neighb <= sep::NEXT; neighb++) {
      if (GetNeighbour(vP, (sep::Axis)ax, (sep::NeighbType)neighb, e_type)) {
        if (e_type == eType) {
          if (i_quant == iIndex) {
            eNeighb = (sep::NeighbType)neighb;
            eAxis = (sep::Axis)ax;
            return true;
          }
          i_quant++;
        }
      }
    }
  }
  return false;
}

bool GridManager::GetNeighbour(Vector2i vP, sep::Axis eAxis,
                               sep::NeighbType eNeighb, sep::CellType& eType) {
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
      InitCellData* p_init_cell = m_pGrid->GetInitCell(vTempPos);
      p_init_cell->m_eType = sep::NORMAL_CELL;
      MacroData& init_cond = p_init_cell->m_cInitCond;
      init_cond.Temperature = GetTemperature();
      init_cond.Concentration = GetConcentration();
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
double GridManager::GetTemperature() {
  if (m_vTemperatureStack.size() < 1)
    return 0.0;
  return m_vTemperatureStack.back();
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
double GridManager::GetConcentration() {
  if (m_vTemperatureStack.size() < 1)
    return 0.0;
  return m_vTemperatureStack.back();
}

void GridManager::FillInGrid() {
  const Vector3i& vGridSize = Config::vGridSize;
  for (int x = 0; x < vGridSize.x(); x++) {
    for (int y = 0; y < vGridSize.y(); y++) {
      for (int z = 0; z < vGridSize.z(); z++) {
        Vector2i v_p(x, y);
        if (m_pGrid->GetInitCell(v_p)->m_eType == sep::EMPTY_CELL)
          continue;
        Cell* p_cell = new Cell();
        m_pGrid->AddCell(std::shared_ptr<Cell>(p_cell));
        m_pGrid->GetInitCell(v_p)->m_pCell = p_cell;
      }
    }
  }
}

void GridManager::LinkCells() {
  const Vector3i& vGridSize = Config::vGridSize;
  for (int x = 0; x < vGridSize.x(); x++) {
    for (int y = 0; y < vGridSize.y(); y++) {
      for (int z = 0; z < vGridSize.z(); z++) {
        Vector2i v_p(x, y);
        if (m_pGrid->GetInitCell(v_p)->m_eType == sep::EMPTY_CELL)
          continue;
        
        sep::NeighbType e_neighb;
        sep::Axis e_ax;
        int i_q;
        // Link to normal
        FindNeighbour(v_p, sep::NORMAL_CELL, e_ax, e_neighb, i_q);
        for (int i = 0; i < i_q; i++) {
          FindNeighbourWithIndex(v_p, sep::NORMAL_CELL, i, e_ax, e_neighb);
          LinkNeighb(v_p, e_ax, e_neighb);
        }
    
        if (m_pGrid->GetInitCell(v_p)->m_eType != sep::FAKE_CELL) {
          // Link to fake
          FindNeighbour(v_p, sep::FAKE_CELL, e_ax, e_neighb, i_q);
          for (int i = 0; i < i_q; i++) {
            FindNeighbourWithIndex(v_p, sep::FAKE_CELL, i, e_ax, e_neighb);
            LinkNeighb(v_p, e_ax, e_neighb);
          }
        }
      }
    }
  }
}

void GridManager::InitCells() {
  const Vector3i& vGridSize = Config::vGridSize;
  for (int x = 0; x < vGridSize.x(); x++) {
    for (int y = 0; y < vGridSize.y(); y++) {
      for (int z = 0; z < vGridSize.z(); z++) {
        Vector2i v_p(x, y);
        if (m_pGrid->GetInitCell(v_p)->m_eType == sep::EMPTY_CELL)
          continue;
        
        Cell* p_cell = m_pGrid->GetInitCell(v_p)->m_pCell;
        InitCellData* p_init_cell = m_pGrid->GetInitCell(v_p);
        const MacroData& init_cond = p_init_cell->m_cInitCond;
        // Set parameters
        Vector3d vAreaStep(0.1, 0.1, 0.1);
        p_cell->setParameters(init_cond.Concentration,
                              init_cond.Temperature,
                              vAreaStep);
        p_cell->Init(this);
      }
    }
  }
}

void GridManager::LinkNeighb(Vector2i vP, sep::Axis eAxis,
                             sep::NeighbType eNeighb) {
  Vector2i v_slash;
  v_slash[eAxis] = GetSlash(eNeighb);
  Vector2i v_target = vP + v_slash;
  Cell* p_target = m_pGrid->GetInitCell(v_target)->m_pCell;
  Cell* p_cell = m_pGrid->GetInitCell(vP)->m_pCell;
  switch (eNeighb) {
    case sep::PREV:
      if (!p_cell->m_vPrev[eAxis].empty())
        throw("Prev is already not emty");
      p_cell->m_vPrev[eAxis].push_back(p_target);
      break;
    case sep::NEXT:
      if (!p_cell->m_vNext[eAxis].empty())
        throw("Next is already not emty");
      p_cell->m_vNext[eAxis].push_back(p_target);
      break;
  }
}





