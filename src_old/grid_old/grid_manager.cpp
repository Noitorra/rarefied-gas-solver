#include <algorithm>
#include <utilities/Utils.h>
#include "parameters/Gas.h"
#include "grid_manager.h"
#include "grid.h"
#include "core/Solver.h"
#include "utilities/Config.h"
#include "utilities/Normalizer.h"
#include "grid/cell.h"
#include "parameters/Impulse.h"


GridManager::GridManager() :
        m_pGrid(new Grid()),
        m_pSolver(nullptr),
        m_pConfig(Config::getInstance()) {

    PushTemperature(1.0);
    PushPressure(1.0);
}

void GridManager::Init(Solver* pSolver) {
    m_pSolver = pSolver;
    ConfigureGrid();
    m_pGrid->Init();
}

void GridManager::PrintGrid() {
    std::cout << "Printing grid..." << std::endl;
    const Vector3i& vGridSize = m_pConfig->getGridSize();
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

void GridManager::PrintLinkage(sep::Axis axis) {
    const Vector3i& vGridSize = m_pConfig->getGridSize();
    for (int z = 0; z < vGridSize.z(); z++) {
        for (int y = 0; y < vGridSize.y(); y++) {
            for (int x = 0; x < vGridSize.x(); x++) {
                Vector2i v_p(x, y);
                if (m_pGrid->GetInitCell(v_p)->m_eType == sep::EMPTY_CELL) {
                    std::cout << "x ";
                    continue;
                }
                std::cout << Utils::asInteger(m_pGrid->GetInitCell(v_p)->m_pCell->m_vType[axis]) << " ";
            }
            std::cout << std::endl;
        }
    }
    std::cout << std::endl;
}

void GridManager::ConfigureGrid() {
    if (m_pConfig->isGPRTGrid()) {
        ConfigureGPRT();
        //ConfigureGPRT2();
        //BoundaryConditionTest();
        //PressureBoundaryConditionTestSmallArea();
        //PressureBoundaryConditionTestBigArea();
    } else {
        ConfigureStandartGrid();
        //ConfigureTestGrid();
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
    if (!m_vBoxes.size()) {
        throw ("no grid boxes");
    }

    // determine grid size
    Vector2i vMin(m_vBoxes[0]->p), vMax;
    std::for_each(m_vBoxes.begin(), m_vBoxes.end(), [&](GridBox* box) {
        if (box->p.x() < vMin.x())
            vMin.x() = box->p.x();
        if (box->p.y() < vMin.y())
            vMin.y() = box->p.y();

        if (box->p.x() + box->size.x() > vMax.x())
            vMax.x() = box->p.x() + box->size.x();
        if (box->p.y() + box->size.y() > vMax.y())
            vMax.y() = box->p.y() + box->size.y();
    });
    Vector2i size = vMax - vMin + Vector2i(2, 2); // size += 2 for fake cells
    m_pConfig->setGridSize(Vector3i(size.x(), size.y(), 1));
    vMin -= Vector2i(1, 1); // min_p -= 1 for fake cells

    m_pGrid->AllocateInitData();

    std::for_each(m_vBoxes.begin(), m_vBoxes.end(), [&](GridBox* box) {
        for (int x = 0; x < box->size.x(); x++) {
            for (int y = 0; y < box->size.y(); y++) {
                Vector2i tmp_pos = box->p + Vector2i(x, y) - vMin;
                InitCellData* init_cell = m_pGrid->GetInitCell(tmp_pos);
                init_cell->m_eType = sep::NORMAL_CELL;
            }
        }

        for (int x = -1; x < box->size.x() + 1; x++) {
            for (int y = -1; y < box->size.y() + 1; y++) {
                Vector2i tmp_pos = box->p + Vector2i(x, y) - vMin;
                InitCellData* init_cell = m_pGrid->GetInitCell(tmp_pos);
                GasesConfigsMap& init_conds = init_cell->m_mInitConds;
                for (int gas = 0; gas < m_pConfig->getGasesNum(); gas++) {
                    init_conds[gas] = box->def_config;
                }
                Vector2i size_with_fakes(box->size);
                size_with_fakes += Vector2i(2, 2);
                box->configFunction(x + 1, y + 1, init_conds, size_with_fakes, box->p);
            }
        }
    });
}

void GridManager::AdoptInitialCells() {
    const Vector3i& vGridSize = m_pConfig->getGridSize();
    for (int x = 0; x < vGridSize.x(); x++) {
        for (int y = 0; y < vGridSize.y(); y++) {
            for (int z = 0; z < vGridSize.z(); z++) {
                Vector2i v_p(x, y);
                InitCellData* init_cell = m_pGrid->GetInitCell(v_p);
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
                                sep::Axis& axis,
                                sep::NeighborType& neighbor,
                                int& quant) {
    int tmp_quant = 0;
    sep::CellType e_type;
    for (int ax = 0; ax <= sep::Y; ax++) {
        for (int neighb = 0; neighb <= sep::NEXT; neighb++) {
            if (GetNeighbour(p, (sep::Axis) ax, (sep::NeighborType) neighb, e_type)) {
                if (e_type == type) {
                    if (!tmp_quant) {
                        neighbor = (sep::NeighborType) neighb;
                        axis = (sep::Axis) ax;
                    }
                    tmp_quant++;
                }
            }
        }
    }
    quant = tmp_quant;
}

bool GridManager::FindNeighbourWithIndex(Vector2i p, sep::CellType type,
                                         int index, sep::Axis& axis,
                                         sep::NeighborType& neighbor) {
    int quant = 0;
    sep::CellType tmp_type;
    for (int ax = 0; ax <= sep::Y; ax++) {
        for (int neighb = 0; neighb <= sep::NEXT; neighb++) {
            if (GetNeighbour(p, (sep::Axis) ax, (sep::NeighborType) neighb, tmp_type)) {
                if (tmp_type == type) {
                    if (quant == index) {
                        neighbor = (sep::NeighborType) neighb;
                        axis = (sep::Axis) ax;
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
                               sep::NeighborType neighbor, sep::CellType& type) {
    Vector2i slash;
    slash[axis] = GetSlash(neighbor);
    p += slash;
    sep::CellType e_type;
    try {
        e_type = m_pGrid->GetInitCell(p)->m_eType;
    } catch (const char* cStr) {
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

void GridManager::AddBox(Vector2d p, Vector2d size, ConfigFunction config_func) {

    if (config_func == nullptr) {
        std::cout << "[GridManager::AddBox] Cannot add null box." << std::endl;
        return;
    }

    CellConfig def_config;
    def_config.dPressure = GetPressure();
    def_config.dTemperature = GetTemperature();
    def_config.dBoundaryTemperature = GetTemperature();

    const Vector2d& cell_size = m_pConfig->getCellSize();

    GridBox* box = new GridBox();
    box->p = Vector2i(p.x() / cell_size.x(), p.y() / cell_size.y());
    box->size = Vector2i(size.x() / cell_size.x(), size.y() / cell_size.y());
    box->def_config = def_config;
    box->configFunction = config_func;

    m_vBoxes.push_back(box);
}

void GridManager::PushTemperature(double t) {
    m_vTemperatures.push_back(t);
}

double GridManager::PopTemperature() {
    if (m_vTemperatures.size() < 1)
        return 0.0;
    double t = m_vTemperatures.back();
    m_vTemperatures.pop_back();
    return t;
}

double GridManager::GetTemperature() {
    if (m_vTemperatures.size() < 1)
        return 0.0;
    return m_vTemperatures.back();
}

void GridManager::PushPressure(double c) {
    m_vPressures.push_back(c);
}

double GridManager::PopPressure() {
    if (m_vPressures.size() < 1)
        return 0.0;
    double c = m_vPressures.back();
    m_vPressures.pop_back();
    return c;
}

double GridManager::GetPressure() {
    if (m_vPressures.size() < 1)
        return 0.0;
    return m_vPressures.back();
}

void GridManager::FillInGrid() {
    const Vector3i& grid_size = m_pConfig->getGridSize();
    for (int x = 0; x < grid_size.x(); x++) {
        for (int y = 0; y < grid_size.y(); y++) {
            for (int z = 0; z < grid_size.z(); z++) {
                Vector2i v_p(x, y);
                if (m_pGrid->GetInitCell(v_p)->m_eType == sep::EMPTY_CELL)
                    continue;
                Cell* p_cell = new Cell();
                m_pGrid->AddCell(p_cell);
                m_pGrid->GetInitCell(v_p)->m_pCell = p_cell;
            }
        }
    }
}

void GridManager::LinkCells() {
    const Vector3i& grid_size = m_pConfig->getGridSize();
    for (int x = 0; x < grid_size.x(); x++) {
        for (int y = 0; y < grid_size.y(); y++) {
            for (int z = 0; z < grid_size.z(); z++) {
                Vector2i v_p(x, y);
                InitCellData* p_init_cell = m_pGrid->GetInitCell(v_p);
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
    const GasVector& gases = m_pConfig->getGases();
    double min_mass = 100.0;
    double max_mass = 0.0;
    std::for_each(gases.begin(), gases.end(), [&](Gas* gas) {
        const double& m = gas->getMass();
        min_mass = std::min(m, min_mass);
        max_mass = std::max(m, max_mass);
    });
    double max_impulse = GetSolver()->GetImpulse()->getMaxImpulse();

    // Set parameters
    Vector2d cell_size = m_pConfig->getCellSize(); // in mm
    cell_size /= 1e3; // in m
    m_pConfig->getNormalizer()->normalize(cell_size.x(), Normalizer::Type::LENGTH);
    m_pConfig->getNormalizer()->normalize(cell_size.y(), Normalizer::Type::LENGTH);
    Vector3d area_step(cell_size.x(), cell_size.y(), 0.1);

    // decreasing time step if needed
    double dTimestep = min_mass * std::min(area_step.x(), area_step.y()) / max_impulse;
    //Config::m_dTimestep = std::min(Config::m_dTimestep, dTimestep);
    m_pConfig->setTimestep(dTimestep);

    //std::cout << "Time step: " << Config::m_dTimestep << " s" << std::endl;
    double dTimestepRestored = m_pConfig->getNormalizer()->restore(static_cast<const double&>(m_pConfig->getTimestep()), Normalizer::Type::TIME);
    std::cout << "Time step: " << dTimestepRestored << " s" << std::endl;


    const Vector3i& vGridSize = m_pConfig->getGridSize();
    for (int x = 0; x < vGridSize.x(); x++) {
        for (int y = 0; y < vGridSize.y(); y++) {
            for (int z = 0; z < vGridSize.z(); z++) {
                Vector2i v_p(x, y);
                if (m_pGrid->GetInitCell(v_p)->m_eType == sep::EMPTY_CELL)
                    continue;

                Cell* p_cell = m_pGrid->GetInitCell(v_p)->m_pCell;
                InitCellData* p_init_cell = m_pGrid->GetInitCell(v_p);

                const GasesConfigsMap& init_conds = p_init_cell->m_mInitConds;
                for (auto val : init_conds) {
                    const int& gas_number = val.first;
                    if (gas_number >= m_pConfig->getGasesNum()) {
                        continue;
                    }
                    const CellConfig& cond = val.second;
                    p_cell->setParameters(
                            cond.dPressure,
                            cond.dTemperature,
                            area_step,
                            gas_number,
                            cond.iLockedAxes
                    );
                    p_cell->setBoundaryType(
                            cond.eBoundaryType,
                            cond.dBoundaryTemperature,
                            cond.vBoundaryFlow,
                            cond.dBoundaryPressure,
                            gas_number
                    );
                }
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
    Cell* target = m_pGrid->GetInitCell(target_pos)->m_pCell;
    Cell* cell = m_pGrid->GetInitCell(p)->m_pCell;
    switch (eNeighbor) {
        case sep::PREV:
            if (cell->m_pPrev[axis] != nullptr)
                throw ("Prev is already not empty");
            cell->m_pPrev[axis] = target;
            break;
        case sep::NEXT:
            if (cell->m_pNext[axis] != nullptr)
                throw ("Next is already not empty");
            cell->m_pNext[axis] = target;
            break;
    }
}

void GridManager::LinkToMyself(Vector2i p, sep::Axis mirror_axis) {
    Cell* cell = m_pGrid->GetInitCell(p)->m_pCell;

    int coord = mirror_axis == sep::X ? p.x() : p.y();
    if (coord < 2) {
        if (cell->m_pPrev[mirror_axis] != nullptr)
            throw ("Next is already not empty");
        cell->m_pPrev[mirror_axis] = cell; // cell->m_vNext[mirror_axis][0]
    } else {
        if (cell->m_pNext[mirror_axis] != nullptr)
            throw ("Next is already not empty");
        cell->m_pNext[mirror_axis] = cell; // cell->m_vPrev[mirror_axis][0]
    }
}

void GridManager::PrintInfo() {
    const Vector3i& vGridSize = m_pConfig->getGridSize();
    std::cout << "Grid size: " << vGridSize.x() << " * " << vGridSize.y() << std::endl;

    const Normalizer* pNormalizer = m_pConfig->getNormalizer();

    pNormalizer->printInfo();

    const double task_size = 0.3; // m
    const double task_size_normalized = pNormalizer->normalize(task_size, Normalizer::Type::LENGTH);
    const double one_travel_time = pNormalizer->restore(task_size_normalized / 1.0, Normalizer::Type::TIME); // s
    const double one_iteration_time = m_pConfig->getNormalizer()->restore(static_cast<const double&>(m_pConfig->getTimestep()), Normalizer::Type::TIME); // s
    const int for_one_travel = one_travel_time / one_iteration_time;
    std::cout << "approximate task size: " << task_size << " m" << std::endl;
    std::cout << "one travel: " << one_travel_time << " s" << std::endl;
    std::cout << "one iteration: " << one_iteration_time << " s" << std::endl;
    std::cout << "for one travel need: " << for_one_travel << " iterations" << std::endl;
    if (for_one_travel < m_pConfig->getMaxIteration()) {
        std::cout << "WARNING: too few iterations for one travel" << std::endl;
    }
    std::cout << std::endl;
}

Grid* GridManager::GetGrid() const {
    return m_pGrid;
}

Solver* GridManager::GetSolver() const {
    return m_pSolver;
}