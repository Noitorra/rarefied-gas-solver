#ifndef RGS_GRID_H
#define RGS_GRID_H

#include "utilities/Types.h"

#include <vector>
#include <unordered_map>

class BaseCell;
class CellConnection;
class Mesh;

class Grid {
private:
    Mesh* _mesh;
    std::vector<std::shared_ptr<BaseCell>> _cells;
    std::unordered_map<int, BaseCell*> _cellsMap;

public:
    explicit Grid(Mesh* mesh);

    Mesh* getMesh() const;

    void addCell(BaseCell* cell);

    const std::vector<std::shared_ptr<BaseCell>>& getCells() const;

    BaseCell* getCellById(int id);

    void init();

    void computeTransfer();

    void computeIntegral(unsigned int gi1, unsigned int gi2);

    void computeBetaDecay(unsigned int gi0, unsigned int gi1, double lambda);

    void check();

    void sync();

};


#endif //RGS_GRID_H
