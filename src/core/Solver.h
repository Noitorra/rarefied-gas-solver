#ifndef RGS_SOLVER_H
#define RGS_SOLVER_H

#include <grid/GridBox.h>
#include <grid/GridConstructor.h>
#include "utilities/Config.h"
#include "parameters/Impulse.h"
#include "grid/Grid.h"

class Cell;

class ResultsWriter;

class GridMaker;

class Solver {
private:
    Config* _config;
    Impulse* _impulse;
    Grid<Cell>* _grid;
    ResultsWriter* _writer;
    GridMaker* _maker;

public:
    explicit Solver(GridConstructor* constructor);

    void init();

    void run();

private:
    void initType();

    void makeTransfer();

    void makeIntegral(unsigned int gi0, unsigned int gi1, double timestep);

    void makeBetaDecay(unsigned int gi0, unsigned int gi1, double lambda);

    void checkCells();
};

#endif //RGS_SOLVER_H
