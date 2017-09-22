#ifndef RGS_GRIDMAKER_H
#define RGS_GRIDMAKER_H

#include <utilities/Types.h>
#include "Grid.h"

class CellData;

class GridBox;

class GridMaker {
public:
    Grid<CellData>* makeGrid(const Vector2u& size);

    void syncGrid(Grid<Cell>* grid);
private:
    Grid<CellData>* makeWholeGrid(const Vector2u& size);

    std::vector<GridBox*> makeBoxes();

    std::vector<Grid<CellData>*> splitGrid(Grid<CellData>* grid, unsigned int numGrids);
};


#endif //RGS_GRIDMAKER_H
