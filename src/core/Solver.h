#ifndef RGS_SOLVER_H
#define RGS_SOLVER_H

#include <grid/GridBox.h>
#include <grid/GridConstructor.h>
#include "utilities/Config.h"
#include "parameters/Impulse.h"
#include "grid/Grid.h"

class Cell;
class ResultsFormatter;
class GridMaker;

class Solver {
private:
    Config* _config;
    Grid* _grid;
    ResultsFormatter* _writer;
    GridMaker* _maker;

public:
    explicit Solver(const std::string& meshfile);

    void init();

    void run();
};

#endif //RGS_SOLVER_H
