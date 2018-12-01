#ifndef RGS_GRID_H
#define RGS_GRID_H

#include "utilities/Types.h"

#include <vector>
#include <unordered_map>

class BaseCell;
class CellConnection;
class Mesh;
class Element;

class Grid {
private:
    Mesh* _mesh;
    std::vector<std::shared_ptr<BaseCell>> _cells;
    std::unordered_map<int, BaseCell*> _cellsMap;

public:
    explicit Grid(Mesh* mesh);

    void init();

    void computeTransfer();

    void computeIntegral(unsigned int gi1, unsigned int gi2);

    void computeBetaDecay(unsigned int gi0, unsigned int gi1, double lambda);

    void check();

    void sync();

    Mesh* getMesh() const;

    void addCell(BaseCell* cell);

    BaseCell* getCellById(int id);

    const std::vector<std::shared_ptr<BaseCell>>& getCells() const;

private:
    void normalizeVolume(Element* element, double& volume);

};


#endif //RGS_GRID_H
