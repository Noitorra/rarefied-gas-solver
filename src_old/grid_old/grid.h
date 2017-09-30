#ifndef GRID_H_
#define GRID_H_

#include "core/Application.h"

class Cell;

class InitCellData {
    friend class GridManager;

    friend class Grid;

    friend class OutResults;

protected:
    InitCellData(sep::CellType eType) :
            m_eType(eType),
            m_pCell(nullptr) {
        m_vMacroData.resize(2);
    };

    sep::CellType m_eType;
    Cell* m_pCell;
    std::vector<MacroData> m_vMacroData;
    GasesConfigsMap m_mInitConds; // initial configs for several gases
};

class Grid {
    friend class GridManager;

public:
    Grid();

    void Init();

    const Vector3i& GetSize() const {
        return m_vSize;
    }

    std::vector<Cell*>& GetCells();

    std::vector<std::vector<std::vector<InitCellData*>>>& GetInitCells();

    InitCellData* GetInitCell(Vector2i vP);

private:
    void AddCell(Cell* pCell);

    void AllocateInitData();

    Vector3i m_vSize;

    std::vector<Cell*> m_vCells;
    std::vector<std::vector<std::vector<InitCellData*>>> m_vInitCells;
};

#endif // GRID_H_
