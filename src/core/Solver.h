#ifndef RGS_SOLVER_H
#define RGS_SOLVER_H

#include "utilities/Config.h"
#include "parameters/Impulse.h"
#include "grid/Grid.h"

class NormalCell;
class Mesh;
class ResultsFormatter;

class Solver {
private:
    std::string _meshfile;
    Config* _config;
    Mesh* _wholeMesh;
    Grid* _grid;
    ResultsFormatter* _formatter;

public:
    explicit Solver(const std::string& meshfile);

    void init();

    void run();
};

#endif //RGS_SOLVER_H
