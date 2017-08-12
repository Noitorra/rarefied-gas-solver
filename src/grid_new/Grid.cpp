#include "Grid.h"
#include "CellWrapper.h"

std::ostream& operator<<(std::ostream& os, const Grid& grid) {
    os << "Size = " << grid._size << std::endl;
    os << "RealCount = " << grid.getRealCount() << std::endl;
    os << "Grid = " << std::endl;
    for (unsigned int y = 0; y < grid._size.y(); y++) {
        for (unsigned int x = 0; x < grid._size.x(); x++) {
            CellWrapper* config = grid.get(x, grid._size.y() - y - 1);
            os << (config == nullptr ? 0 : (config->isSpecial() ? 2 : 1));
        }
        os << std::endl;
    }
    return os;
}
