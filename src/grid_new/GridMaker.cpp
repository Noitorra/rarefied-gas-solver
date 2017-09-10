#include "GridMaker.h"
#include "utilities/parallel.h"
#include "utilities/utils.h"
#include "CellData.h"

#include <iostream>

GridMaker::ConfigBox::ConfigBox(const Vector2d& point, const Vector2d& size, bool isSolid)
    : _point(point), _size(size), _isSolid(isSolid) {}

const Vector2d &GridMaker::ConfigBox::getPoint() const {
    return _point;
}

const Vector2d &GridMaker::ConfigBox::getSize() const {
    return _size;
}

bool GridMaker::ConfigBox::isSolid() const {
    return _isSolid;
}

void GridMaker::ConfigBox::setMainFunction(const GridMaker::SetupFunction& mainFunction) {
    _mainFunction = mainFunction;
}

void GridMaker::ConfigBox::setLeftBorderFunction(const GridMaker::BorderSetupFunction& leftBorderFunction) {
    _leftBorderFunction = leftBorderFunction;
}

void GridMaker::ConfigBox::setRightBorderFunction(const GridMaker::BorderSetupFunction& rightBorderFunction) {
    _rightBorderFunction = rightBorderFunction;
}

void GridMaker::ConfigBox::setTopBorderFunction(const GridMaker::BorderSetupFunction& topBorderFunction) {
    _topBorderFunction = topBorderFunction;
}

void GridMaker::ConfigBox::setBottomBorderFunction(const GridMaker::BorderSetupFunction& bottomBorderFunction) {
    _bottomBorderFunction = bottomBorderFunction;
}

const GridMaker::SetupFunction& GridMaker::ConfigBox::getMainFunction() const {
    return _mainFunction;
}

const GridMaker::BorderSetupFunction& GridMaker::ConfigBox::getLeftBorderFunction() const {
    return _leftBorderFunction;
}

const GridMaker::BorderSetupFunction& GridMaker::ConfigBox::getRightBorderFunction() const {
    return _rightBorderFunction;
}

const GridMaker::BorderSetupFunction& GridMaker::ConfigBox::getTopBorderFunction() const {
    return _topBorderFunction;
}

const GridMaker::BorderSetupFunction& GridMaker::ConfigBox::getBottomBorderFunction() const {
    return _bottomBorderFunction;
}

void GridMaker::ConfigBox::setPoint(const Vector2d& point) {
    _point = point;
}

void GridMaker::ConfigBox::setSize(const Vector2d& size) {
    _size = size;
}

void GridMaker::ConfigBox::setIsSolid(bool isSolid) {
    _isSolid = isSolid;
}

/*
 * How to make a grid? Easy!
 * - setup physical params: size, macro values, etc
 * - normalize data
 *
 * MPI workflow:
 * - main process load all configs, splits them for each process, add's special cells
 * - slave process just loads configs and that's it...
 */
Grid<CellData>* GridMaker::makeGrid(const Vector2u& size, unsigned int gasesCount) {
    Grid<CellData>* grid = nullptr;

    if (Parallel::isUsingMPI() == true) {
        if (Parallel::isMaster() == true) {

            // make configs for whole task
            Grid<CellData>* wholeGrid = makeWholeGrid(size, gasesCount);

            std::cout << "Whole grid:" << std::endl;
            std::cout << *wholeGrid << std::endl;

            // split configs onto several parts, for each process
            std::vector<Grid<CellData>*> splittedGrids = splitGrid(wholeGrid, (unsigned int) Parallel::getSize());

            std::cout << "Splitted grids:" << std::endl;
            for (unsigned int y = 0; y < wholeGrid->getSize().y(); y++) {
                for (unsigned int i = 0; i < splittedGrids.size(); i++) {
                    Grid<CellData>* splitGrid = splittedGrids[i];
                    for (unsigned int x = 0; x < splitGrid->getSize().x(); x++) {
                        CellData* data = splitGrid->get(x, y);

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

                        std::cout << code;
                    }
                    if (i != splittedGrids.size() - 1) {
                        std::cout << " ";
                    }
                }
                std::cout << std::endl;
            }

            // Self configs
            grid = splittedGrids[0];

            // Send to other processes
            for (int processor = 1; processor < Parallel::getSize(); processor++) {
                const char* buffer = Utils::serialize(splittedGrids[processor]);
                Parallel::send(buffer, processor, 100);
            }
        } else {
            const char* buffer = Parallel::receive(0, 100);
            Utils::deserialize(buffer, grid);
        }
    } else {
        grid = makeWholeGrid(size, gasesCount);
        std::cout << *grid << std::endl;
    }

    return grid;
}

Grid<CellData>* GridMaker::makeWholeGrid(const Vector2u& size, unsigned int gasesCount) {

    // create boxes
    std::vector<ConfigBox*> boxes = makeBoxes();

    // find cells field size
    Vector2d lbPoint = boxes.front()->getPoint();
    Vector2d rtPoint = lbPoint;
    for (ConfigBox *box : boxes) {
        const Vector2d &boxPoint = box->getPoint();
        const Vector2d &boxSize = box->getSize();
        if (boxPoint.x() < lbPoint.x()) {
            lbPoint.x() = boxPoint.x();
        }
        if (boxPoint.y() < lbPoint.y()) {
            lbPoint.y() = boxPoint.y();
        }
        if (boxPoint.x() + boxSize.x() > rtPoint.x()) {
            rtPoint.x() = boxPoint.x() + boxSize.x();
        }
        if (boxPoint.y() + boxSize.y() > rtPoint.y()) {
            rtPoint.y() = boxPoint.y() + boxSize.y();
        }
    }
    Vector2d wholeSize = rtPoint - lbPoint; // in original data

    // create configs for whole space
    Grid<CellData>* grid = new Grid<CellData>(size);
    for (auto box : boxes) {
        const Vector2d lbWholeBoxPoint = box->getPoint() - lbPoint;
        const Vector2d rtWholeBoxPoint = lbWholeBoxPoint + box->getSize();

        Vector2u lbWholeBoxGridPoint = {
                (unsigned int) (lbWholeBoxPoint.x() / wholeSize.x() * (size.x() - 2)) + 1,
                (unsigned int) (lbWholeBoxPoint.y() / wholeSize.y() * (size.y() - 2)) + 1
        };

        Vector2u rtWholeBoxGridPoint = {
                (unsigned int) (rtWholeBoxPoint.x() / wholeSize.x() * (size.x() - 2)) + 1,
                (unsigned int) (rtWholeBoxPoint.y() / wholeSize.y() * (size.y() - 2)) + 1
        };

        if (box->isSolid() == false) {
            lbWholeBoxGridPoint.x() += -1;
            lbWholeBoxGridPoint.y() += -1;
            rtWholeBoxGridPoint.x() += 1;
            rtWholeBoxGridPoint.y() += 1;
        }

        const Vector2u wholeBoxGridSize = rtWholeBoxGridPoint - lbWholeBoxGridPoint;

        for (unsigned int x = 0; x < size.x(); x++) {
            for (unsigned int y = 0; y < size.y(); y++) {
                if (box->isSolid() == false) {
                    if (x > lbWholeBoxGridPoint.x() && x < rtWholeBoxGridPoint.x() - 1 && y == lbWholeBoxGridPoint.y()) {
                        auto data = new CellData(CellData::Type::FAKE, gasesCount);
                        if (box->getBottomBorderFunction() != nullptr) {
                            box->getBottomBorderFunction()(1.0 * (x - lbWholeBoxGridPoint.x()) / (rtWholeBoxGridPoint.x() - lbWholeBoxGridPoint.x() - 1), *data);
                        }
                        grid->set(x, y, data);
                    } else if (x > lbWholeBoxGridPoint.x() && x < rtWholeBoxGridPoint.x() - 1 && y == rtWholeBoxGridPoint.y() - 1) {
                        auto data = new CellData(CellData::Type::FAKE, gasesCount);
                        if (box->getTopBorderFunction() != nullptr) {
                            box->getTopBorderFunction()(1.0 * (x - lbWholeBoxGridPoint.x()) / (rtWholeBoxGridPoint.x() - lbWholeBoxGridPoint.x() - 1), *data);
                        }
                        grid->set(x, y, data);
                    } else if (x == lbWholeBoxGridPoint.x() && y > lbWholeBoxGridPoint.y() && y < rtWholeBoxGridPoint.y() - 1) {
                        auto data = new CellData(CellData::Type::FAKE, gasesCount);
                        if (box->getLeftBorderFunction() != nullptr) {
                            box->getLeftBorderFunction()(1.0 * (y - lbWholeBoxGridPoint.y()) / (rtWholeBoxGridPoint.y() - lbWholeBoxGridPoint.y() - 1), *data);
                        }
                        grid->set(x, y, data);
                    } else if (x == rtWholeBoxGridPoint.x() - 1 && y > lbWholeBoxGridPoint.y() && y < rtWholeBoxGridPoint.y() - 1) {
                        auto data = new CellData(CellData::Type::FAKE, gasesCount);
                        if (box->getRightBorderFunction() != nullptr) {
                            box->getRightBorderFunction()(1.0 * (y - lbWholeBoxGridPoint.y()) / (rtWholeBoxGridPoint.y() - lbWholeBoxGridPoint.y() - 1), *data);
                        }
                        grid->set(x, y, data);
                    } else if (x > lbWholeBoxGridPoint.x() && x < rtWholeBoxGridPoint.x() - 1 && y > lbWholeBoxGridPoint.y() && y < rtWholeBoxGridPoint.y() - 1) {
                        auto data = new CellData(CellData::Type::NORMAL, gasesCount);
                        if (box->getMainFunction() != nullptr) {
                            box->getMainFunction()({1.0 * (x - lbWholeBoxGridPoint.x()) / (rtWholeBoxGridPoint.x() - lbWholeBoxGridPoint.x() - 1),
                                                    1.0 * (y - lbWholeBoxGridPoint.y()) / (rtWholeBoxGridPoint.y() - lbWholeBoxGridPoint.y() - 1)}, *data);
                        }
                        grid->set(x, y, data);
                    }
                } else {
                    if (x > lbWholeBoxGridPoint.x() && x < rtWholeBoxGridPoint.x() - 1 && y == lbWholeBoxGridPoint.y()) {
                        auto data = new CellData(CellData::Type::FAKE, gasesCount);
                        if (box->getBottomBorderFunction() != nullptr) {
                            box->getBottomBorderFunction()(1.0 * (x - lbWholeBoxGridPoint.x()) / (rtWholeBoxGridPoint.x() - lbWholeBoxGridPoint.x() - 1), *data);
                        }
                        grid->set(x, y, data);
                    } else if (x > lbWholeBoxGridPoint.x() && x < rtWholeBoxGridPoint.x() - 1 && y == rtWholeBoxGridPoint.y() - 1) {
                        auto data = new CellData(CellData::Type::FAKE, gasesCount);
                        if (box->getTopBorderFunction() != nullptr) {
                            box->getTopBorderFunction()(1.0 * (x - lbWholeBoxGridPoint.x()) / (rtWholeBoxGridPoint.x() - lbWholeBoxGridPoint.x() - 1), *data);
                        }
                        grid->set(x, y, data);
                    } else if (x == lbWholeBoxGridPoint.x() && y > lbWholeBoxGridPoint.y() && y < rtWholeBoxGridPoint.y() - 1) {
                        auto data = new CellData(CellData::Type::FAKE, gasesCount);
                        if (box->getLeftBorderFunction() != nullptr) {
                            box->getLeftBorderFunction()(1.0 * (y - lbWholeBoxGridPoint.y()) / (rtWholeBoxGridPoint.y() - lbWholeBoxGridPoint.y() - 1), *data);
                        }
                        grid->set(x, y, data);
                    } else if (x == rtWholeBoxGridPoint.x() - 1 && y > lbWholeBoxGridPoint.y() && y < rtWholeBoxGridPoint.y() - 1) {
                        auto data = new CellData(CellData::Type::FAKE, gasesCount);
                        if (box->getRightBorderFunction() != nullptr) {
                            box->getRightBorderFunction()(1.0 * (y - lbWholeBoxGridPoint.y()) / (rtWholeBoxGridPoint.y() - lbWholeBoxGridPoint.y() - 1), *data);
                        }
                        grid->set(x, y, data);
                    } else if (x == lbWholeBoxGridPoint.x() && y == lbWholeBoxGridPoint.y()) {
                        auto data = new CellData(CellData::Type::FAKE, gasesCount);
                        if (box->getBottomBorderFunction() != nullptr) {
                            box->getBottomBorderFunction()(1.0 * (x - lbWholeBoxGridPoint.x()) / (rtWholeBoxGridPoint.x() - lbWholeBoxGridPoint.x() - 1), *data);
                        }
                        if (box->getLeftBorderFunction() != nullptr) {
                            box->getLeftBorderFunction()(1.0 * (y - lbWholeBoxGridPoint.y()) / (rtWholeBoxGridPoint.y() - lbWholeBoxGridPoint.y() - 1), *data);
                        }
                        grid->set(x, y, data);
                    } else if (x == lbWholeBoxGridPoint.x() && y == rtWholeBoxGridPoint.y() - 1) {
                        auto data = new CellData(CellData::Type::FAKE, gasesCount);
                        if (box->getLeftBorderFunction() != nullptr) {
                            box->getLeftBorderFunction()(1.0 * (y - lbWholeBoxGridPoint.y()) / (rtWholeBoxGridPoint.y() - lbWholeBoxGridPoint.y() - 1), *data);
                        }
                        if (box->getTopBorderFunction() != nullptr) {
                            box->getTopBorderFunction()(1.0 * (x - lbWholeBoxGridPoint.x()) / (rtWholeBoxGridPoint.x() - lbWholeBoxGridPoint.x() - 1), *data);
                        }
                        grid->set(x, y, data);
                    } else if (x == rtWholeBoxGridPoint.x() - 1 && y == lbWholeBoxGridPoint.y()) {
                        auto data = new CellData(CellData::Type::FAKE, gasesCount);
                        if (box->getBottomBorderFunction() != nullptr) {
                            box->getBottomBorderFunction()(1.0 * (x - lbWholeBoxGridPoint.x()) / (rtWholeBoxGridPoint.x() - lbWholeBoxGridPoint.x() - 1), *data);
                        }
                        if (box->getRightBorderFunction() != nullptr) {
                            box->getRightBorderFunction()(1.0 * (y - lbWholeBoxGridPoint.y()) / (rtWholeBoxGridPoint.y() - lbWholeBoxGridPoint.y() - 1), *data);
                        }
                        grid->set(x, y, data);
                    } else if (x == rtWholeBoxGridPoint.x() - 1 && y == rtWholeBoxGridPoint.y() - 1) {
                        auto data = new CellData(CellData::Type::FAKE, gasesCount);
                        if (box->getTopBorderFunction() != nullptr) {
                            box->getTopBorderFunction()(1.0 * (x - lbWholeBoxGridPoint.x()) / (rtWholeBoxGridPoint.x() - lbWholeBoxGridPoint.x() - 1), *data);
                        }
                        if (box->getRightBorderFunction() != nullptr) {
                            box->getRightBorderFunction()(1.0 * (y - lbWholeBoxGridPoint.y()) / (rtWholeBoxGridPoint.y() - lbWholeBoxGridPoint.y() - 1), *data);
                        }
                        grid->set(x, y, data);
                    } else if (x > lbWholeBoxGridPoint.x() && x < rtWholeBoxGridPoint.x() - 1 && y > lbWholeBoxGridPoint.y() && y < rtWholeBoxGridPoint.y() - 1) {
                        grid->set(x, y, nullptr);
                    }
                }
            }
        }
    }

    return grid;
}

std::vector<GridMaker::ConfigBox*> GridMaker::makeBoxes() {
    std::vector<ConfigBox*> boxes;

    ConfigBox* box0 = new ConfigBox(Vector2d(0.0, 0.0), Vector2d(100.0, 100.0), false);
    boxes.push_back(box0);

    ConfigBox* box1 = new ConfigBox(Vector2d(30.0, 30.0), Vector2d(40.0, 40.0), true);
    boxes.push_back(box1);

    return boxes;
}

std::vector<Grid<CellData>*> GridMaker::splitGrid(Grid<CellData>* grid, unsigned int numGrids) {
    std::vector<Grid<CellData>*> grids;

    unsigned int countNotNull = grid->getCountNotNull();
    unsigned int splitCount = countNotNull / numGrids;
    unsigned int splitIndex = 0;
    unsigned int indexNotNull = 0;
    unsigned int lastIndex = 0;
    for (unsigned int index = 0; index < grid->getCount(); index++) {
        auto data = grid->getByIndex(index);
        if (data != nullptr) {
            indexNotNull++;
        }

        if (indexNotNull == splitCount * (splitIndex + 1) && indexNotNull <= countNotNull - splitCount
            || indexNotNull == countNotNull && splitIndex < numGrids) {

            // get new grid size and shift on X axis
            unsigned int shiftIndex = lastIndex - lastIndex % grid->getSize().y();
            unsigned int sizeX = Grid<CellData>::toPoint(index, grid->getSize()).x() - Grid<CellData>::toPoint(lastIndex, grid->getSize()).x() + 1;

            // make split here
            Grid<CellData>* newGrid = new Grid<CellData>(grid->getSize());
            for (unsigned int k = lastIndex; k <= index; k++) {
                unsigned int newIndex = k - shiftIndex;
                newGrid->setByIndex(newIndex, grid->getByIndex(k));
            }

            // add 1 column to left
            if (splitIndex != 0) {
                sizeX += 1;
                shiftIndex -= grid->getSize().y();
                newGrid->resize(Vector2i(-1, 0), newGrid->getSize() - Vector2u(1, 0));
                for (unsigned int k = lastIndex - grid->getSize().y(); k < lastIndex; k++) {
                    if (grid->getByIndex(k) != nullptr) {
                        auto newData = new CellData(*grid->getByIndex(k));
                        newData->setType(CellData::Type::FAKE_PARALLEL);

                        unsigned int newIndex = k - shiftIndex;
                        newGrid->setByIndex(newIndex, newData);
                    }
                }
            }

            // add 2 column to right
            if (splitIndex != numGrids - 1) {
                sizeX += 2;
                for (unsigned int k = index + 1; k < index + 1 + grid->getSize().y() * 2; k++) {
                    if (grid->getByIndex(k) != nullptr) {
                        auto newData = new CellData(*grid->getByIndex(k));
                        newData->setType(CellData::Type::FAKE_PARALLEL);

                        unsigned int newIndex = k - shiftIndex;
                        newGrid->setByIndex(newIndex, newData);
                    }
                }
            }

            // cut unused space
            newGrid->resize(Vector2i(), Vector2u(sizeX, grid->getSize().y()));
            grids.push_back(newGrid);

            lastIndex = index + 1;
            splitIndex++;
        }
    }

    return grids;
}