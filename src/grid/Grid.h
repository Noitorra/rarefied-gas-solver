#ifndef RGS_GRID_H
#define RGS_GRID_H

#include "utilities/Types.h"
#include "GridBuffer.h"

#include <memory>
#include <vector>
#include <map>

class BaseCell;
class NormalCell;
class BorderCell;
class ParallelCell;
class CellConnection;
class Mesh;
class Element;

class Grid {
private:
    Mesh* _mesh;
    std::vector<std::shared_ptr<BaseCell>> _cells;
    std::map<int, BaseCell*> _cellsMap;
    std::vector<NormalCell*> _normalCells;
    std::vector<BorderCell*> _borderCells;
    std::vector<ParallelCell*> _parallelCells;
    std::shared_ptr<GridBuffer> _buffer;

public:
    explicit Grid(Mesh* mesh);

    void init();

    void computeTransfer();

    void computeIntegral(unsigned int gi1, unsigned int gi2);

    void computeBetaDecay(unsigned int gi0, unsigned int gi1, double lambda);

    void check();

    void sync();

    Mesh* getMesh() const {
        return _mesh;
    }

    BaseCell* getCellById(int id) {
        return _cellsMap[id];
    }

    const std::vector<std::shared_ptr<BaseCell>>& getCells() const {
        return _cells;
    }

    void addCell(BaseCell* cell);

private:
    void normalizeVolume(Element* element, double& volume);

};


#endif //RGS_GRID_H
