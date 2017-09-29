#include "Grid.h"
#include "CellData.h"

std::ostream& operator<<(std::ostream& os, const Grid<CellData>& grid) {
    os << "Grid(Size = " << grid._size << "; CountNotNull = " << grid.getCountNotNull() << "):" << std::endl;
    for (unsigned int y = 0; y < grid._size.y(); y++) {
        for (unsigned int x = 0; x < grid._size.x(); x++) {
            CellData* data = grid.get(x, grid._size.y() - y - 1);

            char code = 'U';
            if (data == nullptr) {
                code = ' ';
            } else if (data->isProcessing() == true) {
                if (data->isFake() == true) {
                    code = '1';
                } else if (data->isNormal() == true) {
                    code = '0';
                }
            } else {
                code = 'S';
            }

            os << code;
        }
        os << std::endl;
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const Grid<Cell>& grid) {
    os << "Grid(Size = " << grid._size << "; CountNotNull = " << grid.getCountNotNull() << "):" << std::endl;
    for (unsigned int y = 0; y < grid._size.y(); y++) {
        const std::vector<Config::Axis>& axis = Config::getInstance()->getAxis();
        for (unsigned int i = 0; i < axis.size(); i++) {
            if (i != 0) {
                os << ' ';
            }

            for (unsigned int x = 0; x < grid._size.x(); x++) {
                Cell* cell = grid.get(x, grid._size.y() - y - 1);

                char code;
                if (cell != nullptr) {
                    code = cell->getComputationTypeAsCode(Utils::asNumber(axis[i]));
                } else {
                    code = ' ';
                }

                os << code;
            }
        }
        os << std::endl;
    }
    return os;
}