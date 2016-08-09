#include "grid.h"
#include "grid_manager.h"
#include "cell.h"
#include "config.h"

Grid::Grid() {}

void Grid::Init() {
    m_vSize = Config::getInstance()->getGridSize();
}

void Grid::AddCell(Cell* pCell) {
    m_vCells.push_back(pCell);
}

std::vector<Cell*>& Grid::GetCells() {
    return m_vCells;
}

std::vector<std::vector<std::vector<InitCellData*>>>& Grid::GetInitCells() {
    return m_vInitCells;
}

InitCellData* Grid::GetInitCell(Vector2i vP) {
    if (vP.x() < 0 || vP.y() < 0 ||
        vP.x() >= m_vSize.x() || vP.y() >= m_vSize.y()) {
        throw ("Wrong position");
    }
    return m_vInitCells[vP.x()][vP.y()][0];
}

void Grid::AllocateInitData() {
    m_vSize = Config::getInstance()->getGridSize();

    for (int x = 0; x < m_vSize.x(); x++) {
        std::vector<std::vector<InitCellData*>> vec_yz;
        m_vInitCells.push_back(vec_yz);

        for (int y = 0; y < m_vSize.y(); y++) {
            std::vector<InitCellData*> vec_z;
            m_vInitCells[x].push_back(vec_z);

            for (int z = 0; z < m_vSize.z(); z++) {
                m_vInitCells[x][y].push_back(new InitCellData(sep::FAKE_CELL)); // Init fake cells, it's ok
                // to see more go to grid_manager.cpp
            }
        }
    }
}
