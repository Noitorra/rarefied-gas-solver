#include "parameters/gas.h"
#include "grid_manager.h"
#include "grid.h"
#include "solver.h"
#include "config.h"
#include "cell.h"
#include "parameters/impulse.h"
#include <algorithm>  // for visual studio compilator

extern double T1, T2, P_sat_T1, P_sat_T2, Q_Xe_in, P_sat_Xe;

GridManager::GridManager() :
  grid_(new Grid),
  solver_(new Solver) {

  PushTemperature(1.0);
  PushPressure(1.0);
}

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
        std::cout << grid_->m_vInitCells[x][y][z]->m_eType << " ";
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
  if (Config::bGPRTGrid) {
    ConfigureGPRT();
	//ConfigureGPRT2();
	//BoundaryConditionTest();
	//PressureBoundaryConditionTestSmallArea();
	//PressureBoundaryConditionTestBigArea();
  } else {
    ConfigureStandartGrid();
  }
  GridGeometryToInitialCells();
  AdoptInitialCells();
//  PrintGrid();

  FillInGrid();
  LinkCells();
  InitCells();
  PrintInfo();
}

void GridManager::GridGeometryToInitialCells() {
  if (!boxes_stack_.size())
    throw("no grid boxes");

  // determine grid size
  Vector2i vMin(boxes_stack_[0].p), vMax;
  std::for_each(boxes_stack_.begin(), boxes_stack_.end(), [&] (GridBox& box) {
    if (box.p.x() < vMin.x())
      vMin.x() = box.p.x();
    if (box.p.y() < vMin.y())
      vMin.y() = box.p.y();

    if (box.p.x() + box.size.x() > vMax.x())
      vMax.x() = box.p.x() + box.size.x();
    if (box.p.y() + box.size.y() > vMax.y())
      vMax.y() = box.p.y() + box.size.y();
  });
  Vector2i size = vMax - vMin + Vector2i(2, 2); // size += 2 for fake cells
  Config::vGridSize = Vector3i(size.x(), size.y(), 1);
  vMin -= Vector2i(1, 1);  // min_p -= 1 for fake cells

  grid_->AllocateInitData();

  std::for_each(boxes_stack_.begin(), boxes_stack_.end(), [&] (GridBox& box) {
    for (int x = 0; x < box.size.x(); x++) {
      for (int y = 0; y < box.size.y(); y++) {
        Vector2i tmp_pos = box.p + Vector2i(x, y) - vMin;
        InitCellData* init_cell = grid_->GetInitCell(tmp_pos);
        init_cell->m_eType = sep::NORMAL_CELL;
      }
    }

    for (int x = -1; x < box.size.x() + 1; x++) {
      for (int y = -1; y < box.size.y() + 1; y++) {
        Vector2i tmp_pos = box.p + Vector2i(x, y) - vMin;
        InitCellData* init_cell = grid_->GetInitCell(tmp_pos);
        GasesConfigsMap& init_conds = init_cell->m_mInitConds;
        for (int gas = 0; gas < Config::iGasesNumber; gas++) {
          init_conds[gas] = box.def_config;
        }
        Vector2i size_with_fakes(box.size);
        size_with_fakes += Vector2i(2, 2);
        box.config_func(x + 1, y + 1, init_conds, size_with_fakes, box.p);
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
        InitCellData* init_cell = grid_->GetInitCell(v_p);
        GasesConfigsMap& init_conds = init_cell->m_mInitConds;
        if (init_cell->m_eType != sep::FAKE_CELL)
          continue;
        
        sep::NeighborType e_neighbor;
        sep::Axis e_ax;
        int i_q;
        // Remove alone fake cells (which without normal neighbour)
        FindNeighbour(v_p, sep::NORMAL_CELL, e_ax, e_neighbor, i_q);
        if (!i_q)
          init_cell->m_eType = sep::EMPTY_CELL;
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
  for (int ax = 0; ax <= sep::Y; ax++) {
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
  for (int ax = 0; ax <= sep::Y; ax++) {
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

void GridManager::SetBox(Vector2d p, Vector2d size, ConfigFunction config_func) {
  CellConfig def_config;
  def_config.pressure = GetPressure();
  def_config.T = GetTemperature();
  def_config.boundary_T = GetTemperature();

  Vector2d& cell_size = Config::vCellSize;
  Vector2i cells_p(p.x() / cell_size.x(), p.y() / cell_size.y());
  Vector2i cells_size(size.x() / cell_size.x(), size.y() / cell_size.y());

  GridBox box;
  box.p = cells_p;
  box.size = cells_size;
  box.def_config = def_config;
  box.config_func = config_func;
  //boxes_stack_.push_back({cells_p, cells_size, def_config, config_func});
  boxes_stack_.push_back(box);
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

void GridManager::PushPressure(double c) {
  pressure_stack_.push_back(c);
}
double GridManager::PopPressure() {
  if (pressure_stack_.size() < 1)
    return 0.0;
  double c = pressure_stack_.back();
  pressure_stack_.pop_back();
  return c;
}
double GridManager::GetPressure() {
  if (pressure_stack_.size() < 1)
    return 0.0;
  return pressure_stack_.back();
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
        InitCellData* p_init_cell = grid_->GetInitCell(v_p);
        const GasesConfigsMap& init_conds = p_init_cell->m_mInitConds;
        if (p_init_cell->m_eType == sep::EMPTY_CELL)
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

        if (p_init_cell->m_eType != sep::FAKE_CELL) {
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
  GasVector& gases = Config::vGas;
  double min_mass = 100.0;
  double max_mass = 0.0;
  std::for_each(gases.begin(), gases.end(), [&] (std::shared_ptr<Gas>& gas) {
      const double& m = gas->getMass();
      min_mass = std::min(m, min_mass);
      max_mass = std::max(m, max_mass);
  });
  double max_impulse = GetSolver()->GetImpulse()->getMaxImpulse();

  const Vector3i& grid_size = Config::vGridSize;
  for (int x = 0; x < grid_size.x(); x++) {
    for (int y = 0; y < grid_size.y(); y++) {
      for (int z = 0; z < grid_size.z(); z++) {
        Vector2i v_p(x, y);
        if (grid_->GetInitCell(v_p)->m_eType == sep::EMPTY_CELL)
          continue;
        
        Cell* p_cell = grid_->GetInitCell(v_p)->m_pCell;
        InitCellData* p_init_cell = grid_->GetInitCell(v_p);

        const GasesConfigsMap& init_conds = p_init_cell->m_mInitConds;
        for (auto val : init_conds) {
          const int& gas_number = val.first;
          if (gas_number >= Config::iGasesNumber)
            continue;
          const CellConfig& cond = val.second;
          p_cell->setParameters(
                  cond.pressure,
                  cond.T,
                  area_step,
                  gas_number
          );
          p_cell->setBoundaryType(
                  cond.boundary_cond,
                  cond.boundary_T,
                  cond.boundary_stream,
                  cond.boundary_pressure,
                  gas_number
          );
        }
        p_cell->Init(this);
      }
    }
  }

  // Set parameters
  Vector2d cell_size = Config::vCellSize;	// in mm
  cell_size /= 1e3;	// in m
  cell_size /= Config::l_normalize;	// normalized
  Vector3d area_step(cell_size.x(), cell_size.y(), 0.1);

  // decreasing time step if needed
  double time_step = min_mass * std::min(area_step.x(), area_step.y()) / max_impulse;
  Config::dTimestep = std::min(Config::dTimestep, time_step);

  //std::cout << "Time step: " << Config::dTimestep << " s" << std::endl;
  std::cout << "Time step: " << Config::dTimestep * Config::tau_normalize << " s" << std::endl;
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

void GridManager::LinkToMyself(Vector2i p, sep::Axis mirror_axis) {
  Cell* cell = grid_->GetInitCell(p)->m_pCell;

  int coord = mirror_axis == sep::X ? p.x() : p.y();
  if (coord < 2) {
    if (!cell->m_vPrev[mirror_axis].empty())
      throw("Next is already not emty");
    cell->m_vPrev[mirror_axis].push_back(cell); // cell->m_vNext[mirror_axis][0]
  } else {
    if (!cell->m_vNext[mirror_axis].empty())
      throw("Next is already not emty");
    cell->m_vNext[mirror_axis].push_back(cell); // cell->m_vPrev[mirror_axis][0]
  }
}

void GridManager::PrintInfo() {
  const Vector3i& grid_size = Config::vGridSize;
  std::cout << "Grid size: " << Config::vGridSize.x() << " x " << Config::vGridSize.y() << std::endl;

  // normalization base
  std::cout << std::endl << "Normalization base:" << std::endl;
  std::cout << "T_normalize: " << Config::T_normalize << " K" << std::endl;
  std::cout << "n_normalize: " << Config::n_normalize << " 1/m^3" << std::endl;
  std::cout << "m_normalize: " << Config::m_normalize << " kg" << std::endl;
  std::cout << "e_cut_normalize: " << Config::e_cut_normalize << " m/s" << std::endl;
  std::cout << "l_normalize: " << Config::l_normalize << " m" << std::endl;
  std::cout << "tau_normalize: " << Config::tau_normalize << " s" << std::endl;
  std::cout << "P_normalize: " << Config::P_normalize << " Pa" << std::endl;
  std::cout << std::endl;

  const double task_size = 0.3; // m
  const double one_travel_time = task_size / Config::e_cut_normalize;  // s
  const double one_iteration_time = Config::tau_normalize * Config::dTimestep;
  const int for_one_travel = one_travel_time / one_iteration_time;
  std::cout << "approximate task size: " << task_size << " m" << std::endl;
  std::cout << "one travel: " << one_travel_time << " s" << std::endl;
  std::cout << "one iteration: " << one_iteration_time << " s" << std::endl;
  std::cout << "for one travel need: " << for_one_travel << " iterations" << std::endl;
  if (for_one_travel < Config::iMaxIteration)
    std::cout << "WARNING: too few iterations for one travel" << std::endl;
  std::cout << std::endl;
}




