#ifndef RGS_GRIDMAKER_H
#define RGS_GRIDMAKER_H

#include <utilities/types.h>
#include "Grid.h"

class CellData;

class GridBox;

class GridMaker {
public:
    static Grid<CellData>* makeGrid(const Vector2u& size);

private:
    static Grid<CellData>* makeWholeGrid(const Vector2u& size);

    static std::vector<GridBox*> makeBoxes();

    static std::vector<Grid<CellData>*> splitGrid(Grid<CellData>* grid, unsigned int numGrids);
};


#endif //RGS_GRIDMAKER_H
