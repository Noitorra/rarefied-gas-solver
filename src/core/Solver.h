#ifndef RGS_SOLVER_H
#define RGS_SOLVER_H

#include "Config.h"
#include "parameters/ImpulseSphere.h"
#include "grid/Grid.h"

class NormalCell;
class ResultsFormatter;

class Solver {
public:
    Solver();

    void init();

    void run();

private:
    Config* _config;
    Grid* _grid;
    ResultsFormatter* _formatter;
};

#endif //RGS_SOLVER_H
