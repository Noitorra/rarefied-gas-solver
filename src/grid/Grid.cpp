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

std::ostream& operator<<(std::ostream& os, const Grid<Cell>& grid) {
    os << "Grid(Size = " << grid._size << "; CountNotNull = " << grid.getCountNotNull() << "):" << std::endl;
    for (unsigned int y = 0; y < grid._size.y(); y++) {
        for (unsigned int x = 0; x < grid._size.x(); x++) {
            Cell* cell = grid.get(x, grid._size.y() - y - 1);

            char code = 'U';
            if (cell != nullptr) {
                switch (cell->getComputationType(0)) {
                    case Cell::ComputationType::NORMAL:
                        code = 'N';
                        break;
                    case Cell::ComputationType::LEFT:
                        code = 'L';
                        break;
                    case Cell::ComputationType::PRE_RIGHT:
                        code = 'P';
                        break;
                    case Cell::ComputationType::RIGHT:
                        code = 'R';
                        break;
                    default:
                        break;
                }
            } else {
                code = ' ';
            }

            os << code;
        }

        os << ' ';

        for (unsigned int x = 0; x < grid._size.x(); x++) {
            Cell* cell = grid.get(x, grid._size.y() - y - 1);

            char code = 'U';
            if (cell != nullptr) {
                switch (cell->getComputationType(1)) {
                    case Cell::ComputationType::NORMAL:
                        code = 'N';
                        break;
                    case Cell::ComputationType::LEFT:
                        code = 'L';
                        break;
                    case Cell::ComputationType::PRE_RIGHT:
                        code = 'P';
                        break;
                    case Cell::ComputationType::RIGHT:
                        code = 'R';
                        break;
                    default:
                        break;
                }
            } else {
                code = ' ';
            }

            os << code;
        }
        os << std::endl;
    }
    return os;
}