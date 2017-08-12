#include "GridMaker.h"
#include "utilities/parallel.h"
#include "utilities/utils.h"
#include "CellWrapper.h"
#include "Grid.h"

#include <boost/range/adaptor/reversed.hpp>
#include <iostream>

GridMaker::ConfigBox::ConfigBox(const Vector2d& point, const Vector2d& size, GridMaker::SetupFunction setupFunction)
        : _point(point), _size(size), _setupFunction(setupFunction) {}

const Vector2d &GridMaker::ConfigBox::getPoint() const {
    return _point;
}

const Vector2d &GridMaker::ConfigBox::getSize() const {
    return _size;
}

const GridMaker::SetupFunction &GridMaker::ConfigBox::getSetupFunction() const {
    return _setupFunction;
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
Grid* GridMaker::makeGrid(const Vector2u& size, unsigned int gasesCount) {

    Grid* grid = nullptr;

    if (Parallel::isUsingMPI() == true) {
        if (Parallel::isMaster() == true) {

            // make configs for whole task
            Grid* wholeGrid = makeWholeGrid(size, gasesCount);

            std::cout << *wholeGrid << std::endl;

            // split configs onto several parts, for each process
            std::vector<Grid*> splitGrids = splitGrid(wholeGrid, (unsigned int) Parallel::getSize());

            // Self configs
            grid = splitGrids[0];

            // Send to other processes
            for (int processor = 1; processor < Parallel::getSize(); processor++) {
                const char* buffer = Utils::serialize(splitGrids[processor]);
                Parallel::send(buffer, processor, 100);
            }

            for (auto* tempGrid : splitGrids) {
                std::cout << *tempGrid << std::endl;
            }
        } else {
            const char* buffer = Parallel::receive(0, 100);
            Utils::deserialize(buffer, grid);
        }
    } else {
        grid = makeWholeGrid(size, gasesCount);
        const char* buffer = Utils::serialize(grid);
        std::cout << *buffer << std::endl;
    }

    return grid;
}

Grid* GridMaker::makeWholeGrid(const Vector2u& size, unsigned int gasesCount) {

    // create boxes
    ConfigBoxVector boxes = makeBoxes();

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
    Grid* grid = new Grid(size);
    for (const ConfigBox* box : boxes | boost::adaptors::reversed) {
        const Vector2d lbWholeBoxPoint = box->getPoint() - lbPoint;
        const Vector2d rtWholeBoxPoint = lbWholeBoxPoint + box->getSize();

        const Vector2u lbWholeBoxGridPoint = {
                (unsigned int) (lbWholeBoxPoint.x() / wholeSize.x() * size.x()),
                (unsigned int) (lbWholeBoxPoint.y() / wholeSize.y() * size.y())
        };

        const Vector2u rtWholeBoxGridPoint = {
                (unsigned int) (rtWholeBoxPoint.x() / wholeSize.x() * size.x()),
                (unsigned int) (rtWholeBoxPoint.y() / wholeSize.y() * size.y())
        };

        const Vector2u wholeBoxGridSize = rtWholeBoxGridPoint - lbWholeBoxGridPoint;

        for (unsigned int x = 0; x < size.x(); x++) {
            for (unsigned int y = 0; y < size.y(); y++) {
                if (x >= lbWholeBoxGridPoint.x() && x < rtWholeBoxGridPoint.x() && y >= lbWholeBoxGridPoint.y() && y < rtWholeBoxGridPoint.y()) {

                    // create and initialize config
                    CellWrapper* config = new CellWrapper(gasesCount);
                    box->getSetupFunction()(lbWholeBoxGridPoint, wholeBoxGridSize, *config);
                    grid->set(x, y, config);
                }
            }
        }
    }

    return grid;
}

GridMaker::ConfigBoxVector GridMaker::makeBoxes() {
    std::vector<GridMaker::ConfigBox*> boxes;

    boxes.push_back(new ConfigBox(Vector2d(0.0, 0.0), Vector2d(20.0, 20.0), [](Vector2u point, Vector2u size, CellWrapper config) {
        // TODO: make a box
    }));

    boxes.push_back(new ConfigBox(Vector2d(30.0, 30.0), Vector2d(20.0, 20.0), [](Vector2u point, Vector2u size, CellWrapper config) {
        // TODO: make a box
    }));

    boxes.push_back(new ConfigBox(Vector2d(15.0, 15.0), Vector2d(20.0, 20.0), [](Vector2u point, Vector2u size, CellWrapper config) {
        // TODO: make a box
    }));

    return boxes;
}

std::vector<Grid*> GridMaker::splitGrid(Grid* grid, unsigned int count) {

    // Calculate indexes where grid will be cut
    unsigned int k = 0;
    unsigned int realCount = 0;
    unsigned int maxEachRealCount = grid->getRealCount() / count;
    std::vector<unsigned int> indexes(count, grid->getCount() - 1);
    for (unsigned int index = 0; index < grid->getCount(); index++) {
        CellWrapper* config = grid->getByIndex(index);
        if (config != nullptr) {
            realCount++;
        }

        if (k != count - 1 && realCount == maxEachRealCount) {
            indexes[k] = index;
            k++;
        }
    }

    // Allocate space
    std::vector<Grid*> splitGrids(count, nullptr);
    for (unsigned int i = 0; i < count; i++) {
        unsigned int indexBegin = i == 0 ? 0 : indexes[i - 1] + 1;
        unsigned int indexEnd = indexes[i];

        Vector2u beginPoint = Grid::toPoint(indexBegin, grid->getSize());
        Vector2u endPoint = Grid::toPoint(indexEnd, grid->getSize());

        Vector2u splitGridPoint = {0, 0};
        Vector2u splitGridSize = {endPoint.x() - beginPoint.x() + 1, grid->getSize().y()};

        // Add 1 column of cells to the left
        if (i != 0) {
            splitGridSize.x() += 1;
            splitGridPoint.x() += 1;
        }

        // Add 2 columns of cells to the right
        if (i != count - 1) {
            splitGridSize.x() += 2;
        }

        splitGrids[i] = new Grid(splitGridSize);

        unsigned int indexStart = Grid::toIndex({beginPoint.x(), 0}, grid->getSize());
        unsigned int indexShift = Grid::toIndex(splitGridPoint, splitGridSize);

        unsigned int indexBeginLeft = Grid::toIndex({beginPoint.x() - 1, beginPoint.y()}, grid->getSize());
        unsigned int indexEndRight = Grid::toIndex({endPoint.x() + 2, endPoint.y()}, grid->getSize());

        for (unsigned int index = 0; index < grid->getCount(); index++) {
            if (index >= indexBeginLeft && index < indexBegin) {
                CellWrapper* config = grid->getByIndex(index);
                if (config != nullptr) {
                    config = new CellWrapper(*config);
                    config->setType(CellWrapper::Type::SPECIAL);
                }
                splitGrids[i]->setByIndex(config, index - indexStart + indexShift);
            }

            if (index >= indexBegin && index <= indexEnd) {
                splitGrids[i]->setByIndex(grid->getByIndex(index), index - indexStart + indexShift);
            }

            if (index > indexEnd && index <= indexEndRight) {
                CellWrapper* config = grid->getByIndex(index);
                if (config != nullptr) {
                    config = new CellWrapper(*config);
                    config->setType(CellWrapper::Type::SPECIAL);
                }
                splitGrids[i]->setByIndex(config, index - indexStart + indexShift);
            }
        }
    }

    return splitGrids;
}
