#include <utility>

#ifndef RGS_GRIDMAKER_H
#define RGS_GRIDMAKER_H

#include <utilities/Types.h>
#include "Grid.h"
#include "GridConstructor.h"

class CellData;

class CellParameters;

class GridBox;

class GridMaker {
public:
    explicit GridMaker(std::string meshfile) : _meshfile(std::move(meshfile)) {}

public:
    Grid* makeGrid();
    void syncGrid(Grid* grid);

private:
    std::string _meshfile;
    std::vector<Grid*> divideGrid(Grid* grid, unsigned int numGrids);
    void updateTimestep();
};


#endif //RGS_GRIDMAKER_H
