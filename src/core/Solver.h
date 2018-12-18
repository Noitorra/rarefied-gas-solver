#ifndef RGS_SOLVER_H
#define RGS_SOLVER_H

#include "Config.h"
#include "parameters/ImpulseSphere.h"
#include "grid/Grid.h"

class NormalCell;
class ResultsFormatter;
class KeyboardManager;

class Solver {
public:
    Solver();

    void init();

    void run();

private:
    Config* _config;
    Grid* _grid;
    ResultsFormatter* _formatter;
    KeyboardManager* _keyboard;
};

#endif //RGS_SOLVER_H
