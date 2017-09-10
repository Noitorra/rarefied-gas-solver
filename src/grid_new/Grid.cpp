#include "Grid.h"
#include "CellData.h"

std::ostream& operator<<(std::ostream& os, const Grid<CellData>& grid) {
    os << "Grid(Size = " << grid._size << "; CountNotNull = " << grid.getCountNotNull() << "):" << std::endl;
    for (unsigned int y = 0; y < grid._size.y(); y++) {
        for (unsigned int x = 0; x < grid._size.x(); x++) {
            CellData* data = grid.get(x, grid._size.y() - y - 1);

            char code = 'X';
            if (data == nullptr) {
                code = ' ';
            } else if (data->getType() == CellData::Type::FAKE) {
                code = '1';
            } else if (data->getType() == CellData::Type::NORMAL) {
                code = '0';
            } else if (data->getType() == CellData::Type::FAKE_PARALLEL) {
                code = 'P';
            }

            os << code;
        }
        os << std::endl;
    }
    return os;
}