#ifndef RGS_GRID_H
#define RGS_GRID_H

#include "utilities/Types.h"

#include <vector>

class Cell;
class CellConnection;
class Mesh;

class Grid {
private:
    std::vector<std::shared_ptr<Cell>> _cells;

public:
    explicit Grid(Mesh* mesh);

    const std::vector<std::shared_ptr<Cell>>& getCells() const;

    Cell* getCellById(int id);

    void computeTransfer();

    void computeIntegral(unsigned int gi1, unsigned int gi2);

    void computeBetaDecay(unsigned int gi0, unsigned int gi1, double lambda);

};


#endif //RGS_GRID_H
