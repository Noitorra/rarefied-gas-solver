#ifndef RGS_GRIDMAKER_H
#define RGS_GRIDMAKER_H

#include <utilities/Types.h>
#include "Grid.h"

class CellData;

class CellParameters;

class GridBox;

class GridMaker {
public:
    enum class SyncType {
        HALF_VALUES,
        VALUES
    };

public:
    Grid<CellData>* makeGrid(const Vector2u& size);

    void syncGrid(Grid<Cell>* grid, SyncType syncType, std::vector<int> syncAxis);

    Grid<CellParameters>* uniteGrids(const std::vector<Grid<CellParameters>*>& grids);

private:
    Grid<CellData>* makeOriginalGrid(const Vector2u& size);

    std::vector<GridBox*> makeBoxes();

    std::vector<Grid<CellData>*> divideGrid(Grid<CellData>* grid, unsigned int numGrids);

    void updateTimestep();
};


#endif //RGS_GRIDMAKER_H
