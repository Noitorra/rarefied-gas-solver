#include "grid_manager.h"
#include "grid.h"
#include "solver.h"
#include "config.h"
#include "cell.h"
#include <algorithm>  // for visual studio compilator

GridManager::GridManager() :
  grid_(new Grid),
  solver_(new Solver) {}

void GridManager::Init() {
  grid_->Init(this);
  solver_->Init(this);
}

void GridManager::PrintGrid() {
  std::cout << "Printing grid..." << std::endl;
  const Vector3i& grid_size = Config::vGridSize;
  for (int z = 0; z < grid_size.z(); z++) {
    for (int y = 0; y < grid_size.y(); y++) {
      for (int x = 0; x < grid_size.x(); x++) {
        std::cout << grid_->m_vInitCells[x][y][z]->m_cInitCond.C << " ";
      }
      std::cout << std::endl;
    }
  }
  std::cout << std::endl;
}

void GridManager::PrintLinkage(sep::Axis axis) {
  const Vector3i& grid_size = Config::vGridSize;
  for (int z = 0; z < grid_size.z(); z++) {
    for (int y = 0; y < grid_size.y(); y++) {
      for (int x = 0; x < grid_size.x(); x++) {
        Vector2i v_p(x, y);
        if (grid_->GetInitCell(v_p)->m_eType == sep::EMPTY_CELL) {
          std::cout << "x ";
          continue;
        }
        std::cout << grid_->GetInitCell(v_p)->m_pCell->m_vType[axis] << " ";
      }
      std::cout << std::endl;
    }
  }
  std::cout << std::endl;
}

void GridManager::ConfigureGrid() {
  ConfigureGridGeometry();
  GridGeometryToInitialCells();
  AdoptInitialCells();
  PrintGrid();

  FillInGrid();
  LinkCells();
  InitCells();
}

void GridManager::GridGeometryToInitialCells() {
  if (!boxes_stack_.size())
    throw("no grid boxes");

  // determine grid size
  Vector2i min(boxes_stack_[0].p), max;
  std::for_each(boxes_stack_.begin(), boxes_stack_.end(), [&] (GridBox& box) {
      if (box.p.x() < min.x())
        min.x() = box.p.x();
      if (box.p.y() < min.y())
        min.y() = box.p.y();

      if (box.p.x() + box.size.x() > max.x())
        max.x() = box.p.x() + box.size.x();
      if (box.p.y() + box.size.y() > max.y())
        max.y() = box.p.y() + box.size.y();
  });
  Vector2i size = max - min;
  Config::vGridSize = Vector3i(size.x(), size.y(), 1);

  grid_->AllocateInitData();

  std::for_each(boxes_stack_.begin(), boxes_stack_.end(), [&] (GridBox& box) {
      for (int x = 0; x < box.size.x(); x++) {
        for (int y = 0; y < box.size.y(); y++) {
          Vector2i vTempPos = box.p + Vector2i(x, y) - min;
          InitCellData* p_init_cell = grid_->GetInitCell(vTempPos);
          p_init_cell->m_eType = sep::NORMAL_CELL;
          CellConfig &init_cond = p_init_cell->m_cInitCond;
          init_cond = box.def_config;
          box.config_func(x, y, &init_cond);
        }
      }
  });
}

void GridManager::AdoptInitialCells() {
  const Vector3i& grid_size = Config::vGridSize;
  for (int x = 0; x < grid_size.x(); x++) {
    for (int y = 0; y < grid_size.y(); y++) {
      for (int z = 0; z < grid_size.z(); z++) {
        Vector2i v_p(x, y);
        if (grid_->GetInitCell(v_p)->m_eType != sep::FAKE_CELL)
          continue;
        
        sep::NeighborType e_neighbor;
        sep::Axis e_ax;
        int i_q;
        // Remove alone fake cells (which without normal neighbour)
        FindNeighbour(v_p, sep::NORMAL_CELL, e_ax, e_neighbor, i_q);
        if (!i_q)
          grid_->GetInitCell(v_p)->m_eType = sep::EMPTY_CELL;
      }
    }
  }
}

// Find the position of first neighbour with given type
// and whole quantity of such neighbours
void GridManager::FindNeighbour(Vector2i p, sep::CellType type,
        sep::Axis&axis,
        sep::NeighborType &neighbor,
        int& quant) {
  int tmp_quant = 0;
  sep::CellType e_type;
  for (int ax = 0; ax <= sep::Z; ax++) {
    for (int neighb = 0; neighb <= sep::NEXT; neighb++) {
      if (GetNeighbour(p, (sep::Axis)ax, (sep::NeighborType)neighb, e_type)) {
        if (e_type == type) {
          if (!tmp_quant) {
            neighbor = (sep::NeighborType)neighb;
            axis = (sep::Axis)ax;
          }
          tmp_quant++;
        }
      }
    }
  }
  quant = tmp_quant;
}

bool GridManager::FindNeighbourWithIndex(Vector2i p, sep::CellType type,
                                int index, sep::Axis&axis,
                                sep::NeighborType &neighbor) {
  int quant = 0;
  sep::CellType tmp_type;
  for (int ax = 0; ax <= sep::Z; ax++) {
    for (int neighb = 0; neighb <= sep::NEXT; neighb++) {
      if (GetNeighbour(p, (sep::Axis)ax, (sep::NeighborType)neighb, tmp_type)) {
        if (tmp_type == type) {
          if (quant == index) {
            neighbor = (sep::NeighborType)neighb;
            axis = (sep::Axis)ax;
            return true;
          }
          quant++;
        }
      }
    }
  }
  return false;
}

bool GridManager::GetNeighbour(Vector2i p, sep::Axis axis,
                               sep::NeighborType neighbor, sep::CellType&type) {
  Vector2i slash;
  slash[axis] = GetSlash(neighbor);
  p += slash;
  sep::CellType e_type;
  try {
    e_type = grid_->GetInitCell(p)->m_eType;
  } catch(const char* cStr) {
    return false;
  }
  type = e_type;
  return true;
}

int GridManager::GetSlash(sep::NeighborType type) const {
  switch (type) {
    case sep::PREV:
      return -1;
    case sep::NEXT:
      return +1;
      default:
      return 0;
  }
}

void GridManager::SetBox(Vector2i p, Vector2i size,
        std::function<void(int x, int y, CellConfig* config)> config_func) {
  CellConfig def_config;
  def_config.C = GetConcentration();
  def_config.T = GetTemperature();
  boxes_stack_.push_back({p, size, def_config, config_func});
}

void GridManager::PushTemperature(double t) {
  temperature_stack_.push_back(t);
}
double GridManager::PopTemperature() {
  if (temperature_stack_.size() < 1)
    return 0.0;
  double t = temperature_stack_.back();
  temperature_stack_.pop_back();
  return t;
}
double GridManager::GetTemperature() {
  if (temperature_stack_.size() < 1)
    return 0.0;
  return temperature_stack_.back();
}

void GridManager::PushConcentration(double c) {
  concentration_stack_.push_back(c);
}
double GridManager::PopConcentration() {
  if (concentration_stack_.size() < 1)
    return 0.0;
  double c = concentration_stack_.back();
  concentration_stack_.pop_back();
  return c;
}
double GridManager::GetConcentration() {
  if (concentration_stack_.size() < 1)
    return 0.0;
  return concentration_stack_.back();
}

void GridManager::FillInGrid() {
  const Vector3i& grid_size = Config::vGridSize;
  for (int x = 0; x < grid_size.x(); x++) {
    for (int y = 0; y < grid_size.y(); y++) {
      for (int z = 0; z < grid_size.z(); z++) {
        Vector2i v_p(x, y);
        if (grid_->GetInitCell(v_p)->m_eType == sep::EMPTY_CELL)
          continue;
        Cell* p_cell = new Cell();
        grid_->AddCell(std::shared_ptr<Cell>(p_cell));
        grid_->GetInitCell(v_p)->m_pCell = p_cell;
      }
    }
  }
}

void GridManager::LinkCells() {
  const Vector3i& grid_size = Config::vGridSize;
  for (int x = 0; x < grid_size.x(); x++) {
    for (int y = 0; y < grid_size.y(); y++) {
      for (int z = 0; z < grid_size.z(); z++) {
        Vector2i v_p(x, y);
        if (grid_->GetInitCell(v_p)->m_eType == sep::EMPTY_CELL)
          continue;
        
        sep::NeighborType neighbor;
        sep::Axis ax;
        int q;
        // Link to normal
        FindNeighbour(v_p, sep::NORMAL_CELL, ax, neighbor, q);
        for (int i = 0; i < q; i++) {
          FindNeighbourWithIndex(v_p, sep::NORMAL_CELL, i, ax, neighbor);
          LinkNeighbors(v_p, ax, neighbor);
        }
    
        if (grid_->GetInitCell(v_p)->m_eType != sep::FAKE_CELL) {
          // Link to fake
          FindNeighbour(v_p, sep::FAKE_CELL, ax, neighbor, q);
          for (int i = 0; i < q; i++) {
            FindNeighbourWithIndex(v_p, sep::FAKE_CELL, i, ax, neighbor);
            LinkNeighbors(v_p, ax, neighbor);
          }
        }
      }
    }
  }
}

void GridManager::InitCells() {
  const Vector3i& grid_size = Config::vGridSize;
  for (int x = 0; x < grid_size.x(); x++) {
    for (int y = 0; y < grid_size.y(); y++) {
      for (int z = 0; z < grid_size.z(); z++) {
        Vector2i v_p(x, y);
        if (grid_->GetInitCell(v_p)->m_eType == sep::EMPTY_CELL)
          continue;
        
        Cell* p_cell = grid_->GetInitCell(v_p)->m_pCell;
        InitCellData* p_init_cell = grid_->GetInitCell(v_p);
        const CellConfig& init_cond = p_init_cell->m_cInitCond;
        // Set parameters
        Vector3d area_step(0.1, 0.1, 0.1);
        p_cell->setParameters(
                init_cond.C,
                init_cond.T,
                area_step
        );
        p_cell->setBoundaryType(init_cond.boundary_cond, init_cond.wall_T);
        p_cell->Init(this);
      }
    }
  }
}

void GridManager::LinkNeighbors(Vector2i p, sep::Axis axis,
        sep::NeighborType eNeighbor) {
  Vector2i v_slash;
  v_slash[axis] = GetSlash(eNeighbor);
  Vector2i target_pos = p + v_slash;
  Cell* target = grid_->GetInitCell(target_pos)->m_pCell;
  Cell* cell = grid_->GetInitCell(p)->m_pCell;
  switch (eNeighbor) {
    case sep::PREV:
      if (!cell->m_vPrev[axis].empty())
        throw("Prev is already not emty");
      cell->m_vPrev[axis].push_back(target);
      break;
    case sep::NEXT:
      if (!cell->m_vNext[axis].empty())
        throw("Next is already not emty");
      cell->m_vNext[axis].push_back(target);
      break;
  }
}





