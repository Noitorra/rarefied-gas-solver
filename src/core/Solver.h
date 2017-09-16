#ifndef RGS_SOLVER_H
#define RGS_SOLVER_H

#include "main.h"
#include "utilities/config.h"
#include "parameters/impulse.h"
#include "grid/Grid.h"

class Cell;

class Solver {
private:
    Config* _config;
    Impulse* _impulse;
    Grid<Cell>* _grid;

public:
    Solver() = default;

    void init(Grid<CellData>* grid);

    void run();

private:
    void initCellType(sep::Axis axis);

    void makeStep(sep::Axis axis);

    void makeIntegral(unsigned int gi0, unsigned int gi1, double timestep);

    void makeBetaDecay(unsigned int gi0, unsigned int gi1, double lambda);

    void checkCells();
};

#endif //RGS_SOLVER_H
