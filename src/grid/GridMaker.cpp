#include "GridMaker.h"
#include "utilities/Parallel.h"
#include "utilities/Utils.h"
#include "CellData.h"
#include "parameters/Impulse.h"

#include <iostream>

GridMaker::GridMaker(GridConstructor* constructor) {
    _constructor = constructor;
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
Grid<CellData>* GridMaker::makeGrid(const Vector2u& size) {
    Grid<CellData>* grid = nullptr;

    if (Parallel::isUsingMPI() == true && Parallel::getSize() > 1) {
        if (Parallel::isMaster() == true) {

            // make configs for original task
            Grid<CellData>* originalGrid = makeOriginalGrid(size);

            std::cout << "Original grid:" << std::endl << *originalGrid << std::endl;

            // update config time
            updateTimestep();

            // split configs onto several parts, for each process
            std::vector<Grid<CellData>*> grids = divideGrid(originalGrid, (unsigned int) Parallel::getSize());

            std::cout << "Divided grids:" << std::endl;
            for (unsigned int y = originalGrid->getSize().y() - 1; y + 1 > 0; y--) {
                for (unsigned int i = 0; i < grids.size(); i++) {
                    Grid<CellData>* dividedGrid = grids[i];
                    for (unsigned int x = 0; x < dividedGrid->getSize().x(); x++) {
                        CellData* data = dividedGrid->get(x, y);

                        char code = 'U';
                        if (data == nullptr) {
                            code = ' ';
                        } else if (data->isProcessing() == true) {
                            if (data->getType() == CellData::Type::FAKE) {
                                code = '1';
                            } else if (data->getType() == CellData::Type::NORMAL) {
                                code = '0';
                            }
                        } else {
                            code = 'S';
                        }
                        std::cout << code;
                    }
                    if (i != grids.size() - 1) {
                        std::cout << "-";
                    }
                }
                std::cout << std::endl;
            }

            // Self configs
            grid = grids[0];

            // Send to other processes
            for (int processor = 1; processor < Parallel::getSize(); processor++) {
                Parallel::send(Utils::serialize(grids[processor]), processor, Parallel::COMMAND_GRID);
                delete(grids[processor]);
            }
        } else {
            // update config time
            updateTimestep();

            Utils::deserialize(Parallel::recv(0, Parallel::COMMAND_GRID), grid);
        }
    } else {
        grid = makeOriginalGrid(size);
        std::cout << *grid << std::endl;

        // update config time
        updateTimestep();
    }

    return grid;
}

Grid<CellData>* GridMaker::makeOriginalGrid(const Vector2u& size) {

    // create boxes
    std::vector<GridBox*> boxes = _constructor->createBoxes();

    // find cells field size
    Vector2d lbPoint = boxes.front()->getPoint();
    Vector2d rtPoint = lbPoint;
    for (GridBox *box : boxes) {
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
    Vector2d originalSize = rtPoint - lbPoint; // in original data

    // create configs for original space
    Grid<CellData>* grid = new Grid<CellData>(size);
    for (auto box : boxes) {
        const Vector2d lbOriginalBoxPoint = box->getPoint() - lbPoint;
        const Vector2d rtOriginalBoxPoint = lbOriginalBoxPoint + box->getSize();

        Vector2u lbOriginalBoxGridPoint = {
                (unsigned int) (lbOriginalBoxPoint.x() / originalSize.x() * (size.x() - 2)) + 1,
                (unsigned int) (lbOriginalBoxPoint.y() / originalSize.y() * (size.y() - 2)) + 1
        };

        Vector2u rtOriginalBoxGridPoint = {
                (unsigned int) (rtOriginalBoxPoint.x() / originalSize.x() * (size.x() - 2)) + 1,
                (unsigned int) (rtOriginalBoxPoint.y() / originalSize.y() * (size.y() - 2)) + 1
        };

        if (box->isSolid() == false) {
            lbOriginalBoxGridPoint.x() += -1;
            lbOriginalBoxGridPoint.y() += -1;
            rtOriginalBoxGridPoint.x() += 1;
            rtOriginalBoxGridPoint.y() += 1;
        }

        const Vector2u originalBoxGridSize = rtOriginalBoxGridPoint - lbOriginalBoxGridPoint;

        for (unsigned int x = 0; x < size.x(); x++) {
            for (unsigned int y = 0; y < size.y(); y++) {
                if (box->isSolid() == false) {
                    Vector2d point{
                            1.0 * (x - lbOriginalBoxGridPoint.x()) / (rtOriginalBoxGridPoint.x() - lbOriginalBoxGridPoint.x() - 1),
                            1.0 * (y - lbOriginalBoxGridPoint.y()) / (rtOriginalBoxGridPoint.y() - lbOriginalBoxGridPoint.y() - 1)
                    };

                    if (x > lbOriginalBoxGridPoint.x() && x < rtOriginalBoxGridPoint.x() - 1 && y == lbOriginalBoxGridPoint.y()) {
                        auto data = new CellData(CellData::Type::FAKE);
                        if (box->getBottomBorderFunction() != nullptr) {
                            box->getBottomBorderFunction()(point.x(), *data);
                        }
                        grid->set(x, y, data);
                    } else if (x > lbOriginalBoxGridPoint.x() && x < rtOriginalBoxGridPoint.x() - 1 && y == rtOriginalBoxGridPoint.y() - 1) {
                        auto data = new CellData(CellData::Type::FAKE);
                        if (box->getTopBorderFunction() != nullptr) {
                            box->getTopBorderFunction()(point.x(), *data);
                        }
                        grid->set(x, y, data);
                    } else if (x == lbOriginalBoxGridPoint.x() && y > lbOriginalBoxGridPoint.y() && y < rtOriginalBoxGridPoint.y() - 1) {
                        auto data = new CellData(CellData::Type::FAKE);
                        if (box->getLeftBorderFunction() != nullptr) {
                            box->getLeftBorderFunction()(point.y(), *data);
                        }
                        grid->set(x, y, data);
                    } else if (x == rtOriginalBoxGridPoint.x() - 1 && y > lbOriginalBoxGridPoint.y() && y < rtOriginalBoxGridPoint.y() - 1) {
                        auto data = new CellData(CellData::Type::FAKE);
                        if (box->getRightBorderFunction() != nullptr) {
                            box->getRightBorderFunction()(point.y(), *data);
                        }
                        grid->set(x, y, data);
                    } else if (x > lbOriginalBoxGridPoint.x() && x < rtOriginalBoxGridPoint.x() - 1 && y > lbOriginalBoxGridPoint.y() && y < rtOriginalBoxGridPoint.y() - 1) {
                        auto data = new CellData(CellData::Type::NORMAL);
                        if (box->getMainFunction() != nullptr) {
                            box->getMainFunction()(point, *data);
                        }
                        grid->set(x, y, data);
                    }
                } else {
                    Vector2d point{
                            1.0 * (x - lbOriginalBoxGridPoint.x()) / (rtOriginalBoxGridPoint.x() - lbOriginalBoxGridPoint.x() - 1),
                            1.0 * (y - lbOriginalBoxGridPoint.y()) / (rtOriginalBoxGridPoint.y() - lbOriginalBoxGridPoint.y() - 1)
                    };

                    if (x > lbOriginalBoxGridPoint.x() && x < rtOriginalBoxGridPoint.x() - 1 && y == lbOriginalBoxGridPoint.y()) {
                        auto data = new CellData(CellData::Type::FAKE);
                        if (box->getBottomBorderFunction() != nullptr) {
                            box->getBottomBorderFunction()(point.x(), *data);
                        }
                        grid->set(x, y, data);
                    } else if (x > lbOriginalBoxGridPoint.x() && x < rtOriginalBoxGridPoint.x() - 1 && y == rtOriginalBoxGridPoint.y() - 1) {
                        auto data = new CellData(CellData::Type::FAKE);
                        if (box->getTopBorderFunction() != nullptr) {
                            box->getTopBorderFunction()(point.x(), *data);
                        }
                        grid->set(x, y, data);
                    } else if (x == lbOriginalBoxGridPoint.x() && y > lbOriginalBoxGridPoint.y() && y < rtOriginalBoxGridPoint.y() - 1) {
                        auto data = new CellData(CellData::Type::FAKE);
                        if (box->getLeftBorderFunction() != nullptr) {
                            box->getLeftBorderFunction()(point.y(), *data);
                        }
                        grid->set(x, y, data);
                    } else if (x == rtOriginalBoxGridPoint.x() - 1 && y > lbOriginalBoxGridPoint.y() && y < rtOriginalBoxGridPoint.y() - 1) {
                        auto data = new CellData(CellData::Type::FAKE);
                        if (box->getRightBorderFunction() != nullptr) {
                            box->getRightBorderFunction()(point.y(), *data);
                        }
                        grid->set(x, y, data);
                    } else if (x == lbOriginalBoxGridPoint.x() && y == lbOriginalBoxGridPoint.y()) {
                        auto data = new CellData(CellData::Type::FAKE);
                        if (box->getBottomBorderFunction() != nullptr) {
                            box->getBottomBorderFunction()(point.x(), *data);
                        }
                        if (box->getLeftBorderFunction() != nullptr) {
                            box->getLeftBorderFunction()(point.y(), *data);
                        }
                        grid->set(x, y, data);
                    } else if (x == lbOriginalBoxGridPoint.x() && y == rtOriginalBoxGridPoint.y() - 1) {
                        auto data = new CellData(CellData::Type::FAKE);
                        if (box->getLeftBorderFunction() != nullptr) {
                            box->getLeftBorderFunction()(point.y(), *data);
                        }
                        if (box->getTopBorderFunction() != nullptr) {
                            box->getTopBorderFunction()(point.x(), *data);
                        }
                        grid->set(x, y, data);
                    } else if (x == rtOriginalBoxGridPoint.x() - 1 && y == lbOriginalBoxGridPoint.y()) {
                        auto data = new CellData(CellData::Type::FAKE);
                        if (box->getBottomBorderFunction() != nullptr) {
                            box->getBottomBorderFunction()(point.x(), *data);
                        }
                        if (box->getRightBorderFunction() != nullptr) {
                            box->getRightBorderFunction()(point.y(), *data);
                        }
                        grid->set(x, y, data);
                    } else if (x == rtOriginalBoxGridPoint.x() - 1 && y == rtOriginalBoxGridPoint.y() - 1) {
                        auto data = new CellData(CellData::Type::FAKE);
                        if (box->getTopBorderFunction() != nullptr) {
                            box->getTopBorderFunction()(point.x(), *data);
                        }
                        if (box->getRightBorderFunction() != nullptr) {
                            box->getRightBorderFunction()(point.y(), *data);
                        }
                        grid->set(x, y, data);
                    } else if (x > lbOriginalBoxGridPoint.x() && x < rtOriginalBoxGridPoint.x() - 1 && y > lbOriginalBoxGridPoint.y() && y < rtOriginalBoxGridPoint.y() - 1) {
                        grid->set(x, y, nullptr);
                    }
                }
            }
        }
    }

    return grid;
}

std::vector<Grid<CellData>*> GridMaker::divideGrid(Grid<CellData>* grid, unsigned int numGrids) {
    std::vector<Grid<CellData>*> dividedGrids;

    unsigned int countNotNull = grid->getCountNotNull();
    unsigned int splitCount = countNotNull / numGrids;
    unsigned int splitIndex = 0;
    unsigned int indexNotNull = 0;
    unsigned int lastIndex = 0;
    for (unsigned int index = 0; index < grid->getCount(); index++) {
        auto* data = grid->getByIndex(index);
        if (data != nullptr) {
            indexNotNull++;
        }

        if (indexNotNull >= splitCount * (splitIndex + 1) && indexNotNull < countNotNull || indexNotNull == countNotNull && splitIndex < numGrids) {

            std::cout << "Index = " << index << " MaxIndex = " << grid->getCount() << std::endl;

            if (data != nullptr) {
                if (data->isNormal() == true) {
                    if (index > 0) {
                        auto* prevData = grid->getByIndex(index - 1);
                        if (prevData != nullptr && prevData->isFake() == true) {
                            continue;
                        }
                    }

                    if (index < grid->getCount() - 1) {
                        auto* nextData = grid->getByIndex(index + 1);
                        if (nextData != nullptr && nextData->isFake() == true) {
                            continue;
                        }
                    }
                } else {
                    if (index > 0) {
                        auto* prevData = grid->getByIndex(index - 1);
                        if (prevData != nullptr && prevData->isNormal() == true) {
                            continue;
                        }
                    }

                    if (index < grid->getCount() - 1) {
                        auto* nextData = grid->getByIndex(index + 1);
                        if (nextData != nullptr && nextData->isNormal() == true) {
                            continue;
                        }
                    }
                }
            }

            std::cout << "DONE" << std::endl;

            Vector2u point = Grid<CellData>::toPoint(index, grid->getSize());
            Vector2u lastPoint = Grid<CellData>::toPoint(lastIndex, grid->getSize());

            // get new grid size and shift on X axis
            unsigned int shiftIndex = lastIndex - lastIndex % grid->getSize().y();
            unsigned int sizeX = point.x() - lastPoint.x() + 1;

            // make split here
            Grid<CellData>* newGrid = new Grid<CellData>(grid->getSize());
            for (unsigned int k = lastIndex; k <= index; k++) {
                auto* kData = grid->getByIndex(k);
                if (kData != nullptr) {
                    kData->setId(k); // id from original grid

                    newGrid->setByIndex(k - shiftIndex, kData);
                }
            }

            // add 1 column to left
            if (splitIndex > 0) {
                unsigned int addX = 1;
                for (unsigned int k = lastIndex; k < lastIndex + grid->getSize().y(); k++) {
                    auto* kData = grid->getByIndex(k);
                    auto* kPrevData = grid->getByIndex(k - grid->getSize().y());
                    if (kData != nullptr && kData->isFake() == true && kPrevData != nullptr && kPrevData->isNormal() == true) {
                        addX += 1;
                        break;
                    }
                }

                sizeX += addX;
                shiftIndex -= addX * grid->getSize().y();
                newGrid->resize(Vector2i(-addX, 0), newGrid->getSize() - Vector2u(addX, 0));
                for (unsigned int k = lastIndex - grid->getSize().y(); k < lastIndex; k++) {
                    auto* kData = grid->getByIndex(k);
                    if (kData != nullptr) {
                        if (kData->isFake() == true) {
                            auto* kNextData = grid->getByIndex(k + grid->getSize().y());
                            if (kNextData == nullptr || kNextData->isFake() == true) {
                                continue;
                            }
                        }

                        auto* newData = new CellData(*kData);
                        newData->setProcessing(false);
                        newData->setSyncId(k);
                        newData->setSyncProcessorRank(splitIndex - 1);
                        newData->setSyncType(CellData::SyncType::ALL);

                        unsigned int newIndex = k - shiftIndex;
                        newGrid->setByIndex(newIndex, newData);
                        newData->setId(-newIndex); // if from new grid (negative)
                    }
                }
                for (unsigned int k = lastIndex; k < lastIndex + grid->getSize().y(); k++) {

                    // get current data
                    auto* kData = grid->getByIndex(k);
                    if (kData != nullptr && kData->isFake() == true) {

                        // get prev data
                        unsigned int kPrev = k - grid->getSize().y();
                        auto* kPrevData = grid->getByIndex(kPrev);
                        if (kPrevData != nullptr && kPrevData->isNormal() == true) {

                            // get prev data from newGrid
                            auto* newPrevData = newGrid->getByIndex(kPrev - shiftIndex);
                            newPrevData->setSyncType(CellData::SyncType::VALUES);
                            std::cout << "ValuesData id = " << newPrevData->getId() << " syncId = " << newPrevData->getSyncId() << std::endl;

                            // get prev prev data
                            unsigned int kPrevPrev = kPrev - grid->getSize().y();
                            auto* kPrevPrevData = grid->getByIndex(kPrevPrev);
                            if (kPrevPrevData != nullptr && kPrevPrevData->isNormal() == true) {
                                auto* newData = new CellData(*kPrevPrevData);
                                newData->setProcessing(false);
                                newData->setSyncId(kPrevPrev);
                                newData->setSyncProcessorRank(splitIndex - 1);
                                newData->setSyncType(CellData::SyncType::ALL);

                                unsigned int newIndex = kPrevPrev - shiftIndex;
                                newGrid->setByIndex(newIndex, newData);
                                newData->setId(-newIndex); // if from new grid (negative)
                            }
                        }
                    }
                }
            }

            // add 2 column to right
            if (splitIndex < numGrids - 1) {
                sizeX += 2;
                for (unsigned int k = index + 1; k < index + 1 + grid->getSize().y() * 2; k++) {
                    auto* kData = grid->getByIndex(k);
                    if (kData != nullptr) {
                        if (kData->isFake() == true) {
                            unsigned int kPrev = k - grid->getSize().y();
                            auto* kPrevData = grid->getByIndex(kPrev);
                            if (kPrevData != nullptr && kPrevData->isNormal() == true) {
                                unsigned int nextLastIndex = index + 1;
                                unsigned int nextShiftIndex = nextLastIndex - nextLastIndex % grid->getSize().y();
                                nextShiftIndex -= 2 * grid->getSize().y();
                                unsigned int newIndexPrev = kPrev - nextShiftIndex;

                                auto* newPrevData = newGrid->getByIndex(kPrev - shiftIndex);
                                newPrevData->setSyncId(-newIndexPrev);
                                newPrevData->setSyncProcessorRank(splitIndex + 1);
                                newPrevData->setSyncAxis(Utils::asNumber(Config::Axis::X));
                                newPrevData->setSyncType(CellData::SyncType::HALF_VALUES);
                                std::cout << "HalfValuesData id = " << newPrevData->getId() << " syncId = " << newPrevData->getSyncId() << std::endl;
                            }
                        }

                        Vector2u point = Grid<CellData>::toPoint(k, grid->getSize());
                        auto* kPrevDataX = point.x() > 0 ? grid->get(point.x() - 1, point.y()) : nullptr;
                        auto* kPrevDataY = point.y() > 0 ? grid->get(point.x(), point.y() - 1) : nullptr;
                        if (kData->isNormal() == true && (kPrevDataX != nullptr && kPrevDataX->isNormal() == true || kPrevDataY != nullptr && kPrevDataY->isNormal() == true)) {
                            auto* newData = new CellData(*kData);
                            newData->setProcessing(false);
                            newData->setSyncId(k);
                            newData->setSyncProcessorRank(splitIndex + 1);
                            newData->setSyncType(CellData::SyncType::ALL);

                            unsigned int newIndex = k - shiftIndex;
                            newGrid->setByIndex(newIndex, newData);
                            newData->setId(-newIndex); // if from new grid (negative)
                        }
                    }
                }
            }

            // cut unused space
            newGrid->resize(Vector2i(), Vector2u(sizeX, grid->getSize().y()));
            dividedGrids.push_back(newGrid);

            lastIndex = index + 1;
            splitIndex++;
        }
    }

    return dividedGrids;
}

void GridMaker::syncGrid(Grid<Cell>* grid, SyncType syncType, std::vector<int> syncAxis) {

    // need vectors of ids
    std::vector<int> sendNextIds;
    std::vector<int> sendPrevIds;
    std::vector<int> recvNextIds;
    std::vector<int> recvPrevIds;

    // need vectors of cells
    std::vector<Cell*> sendNextCells;
    std::vector<Cell*> sendPrevCells;
    std::vector<Cell*> recvNextCells;
    std::vector<Cell*> recvPrevCells;

    int processor = Parallel::getRank();
    int size = Parallel::getSize();

    // fill send vectors
    for (auto& cell : grid->getValues()) {
        if (cell != nullptr) {
            if (syncType == SyncType::HALF_VALUES) {
                if (cell->getData()->isSyncHalfValues() == false) {
                    continue;
                }
            } else if (syncType == SyncType::VALUES) {
                if (cell->getData()->isSyncValues() == false) {
                    continue;
                }
            }
            bool isSyncAvaliableOnAxis = true;
            for (auto axis : syncAxis) {
                if (cell->getData()->isSyncAvaliable(axis) == false) {
                    isSyncAvaliableOnAxis = false;
                    break;
                }
            }
            if (isSyncAvaliableOnAxis == false) {
                continue;
            }

            int originalProcessor = cell->getData()->getSyncProcessorRank();
            if (originalProcessor > processor) {
                sendNextIds.push_back(cell->getData()->getSyncId());
                recvNextCells.push_back(cell);
            } else if (originalProcessor < processor) {
                sendPrevIds.push_back(cell->getData()->getSyncId());
                recvPrevCells.push_back(cell);
            }
        }
    }

    // parallel exchange for ids
    if (processor % 2 == 0) {
        if (processor < size - 1) {
            Parallel::send(Utils::serialize(sendNextIds), processor + 1, Parallel::COMMAND_SYNC_IDS);
            Utils::deserialize(Parallel::recv(processor + 1, Parallel::COMMAND_SYNC_IDS), recvNextIds);
        }
        if (processor > 0) {
            Utils::deserialize(Parallel::recv(processor - 1, Parallel::COMMAND_SYNC_IDS), recvPrevIds);
            Parallel::send(Utils::serialize(sendPrevIds), processor - 1, Parallel::COMMAND_SYNC_IDS);
        }
    } else {
        if (processor > 0) {
            Utils::deserialize(Parallel::recv(processor - 1, Parallel::COMMAND_SYNC_IDS), recvPrevIds);
            Parallel::send(Utils::serialize(sendPrevIds), processor - 1, Parallel::COMMAND_SYNC_IDS);
        }
        if (processor < size - 1) {
            Parallel::send(Utils::serialize(sendNextIds), processor + 1, Parallel::COMMAND_SYNC_IDS);
            Utils::deserialize(Parallel::recv(processor + 1, Parallel::COMMAND_SYNC_IDS), recvNextIds);
        }
    }

    // fill recv vectors
    for (auto& id : recvNextIds) {
        for (auto& cell : grid->getValues()) {
            if (cell != nullptr) {
                if (id == cell->getData()->getId()) {
                    sendNextCells.push_back(cell);
                }
            }
        }
    }
    for (auto& id : recvPrevIds) {
        for (auto& cell : grid->getValues()) {
            if (cell != nullptr) {
                if (id == cell->getData()->getId()) {
                    sendPrevCells.push_back(cell);
                }
            }
        }
    }

    switch (syncType) {
        case SyncType::HALF_VALUES:

            // parallel exchange for half values
            if (processor % 2 == 0) {
                if (processor < size - 1) {
                    for (auto& cell : sendNextCells) {
                        Parallel::send(Utils::serialize(cell->getHalfValues()), processor + 1, Parallel::COMMAND_SYNC_HALF_VALUES);
                    }
                    for (auto& cell : recvNextCells) {
                        Utils::deserialize(Parallel::recv(processor + 1, Parallel::COMMAND_SYNC_HALF_VALUES), cell->getHalfValues());
                    }
                }
                if (processor > 0) {
                    for (auto& cell : recvPrevCells) {
                        Utils::deserialize(Parallel::recv(processor - 1, Parallel::COMMAND_SYNC_HALF_VALUES), cell->getHalfValues());
                    }
                    for (auto& cell : sendPrevCells) {
                        Parallel::send(Utils::serialize(cell->getHalfValues()), processor - 1, Parallel::COMMAND_SYNC_HALF_VALUES);
                    }
                }
            } else {
                if (processor > 0) {
                    for (auto& cell : recvPrevCells) {
                        Utils::deserialize(Parallel::recv(processor - 1, Parallel::COMMAND_SYNC_HALF_VALUES), cell->getHalfValues());
                    }
                    for (auto& cell : sendPrevCells) {
                        Parallel::send(Utils::serialize(cell->getHalfValues()), processor - 1, Parallel::COMMAND_SYNC_HALF_VALUES);
                    }
                }
                if (processor < size - 1) {
                    for (auto& cell : sendNextCells) {
                        Parallel::send(Utils::serialize(cell->getHalfValues()), processor + 1, Parallel::COMMAND_SYNC_HALF_VALUES);
                    }
                    for (auto& cell : recvNextCells) {
                        Utils::deserialize(Parallel::recv(processor + 1, Parallel::COMMAND_SYNC_HALF_VALUES), cell->getHalfValues());
                    }
                }
            }
            break;
        case SyncType::VALUES:

            // parallel exchange for normal values
            if (processor % 2 == 0) {
                if (processor < size - 1) {
                    for (auto& cell : sendNextCells) {
                        Parallel::send(Utils::serialize(cell->getValues()), processor + 1, Parallel::COMMAND_SYNC_VALUES);
                    }
                    for (auto& cell : recvNextCells) {
                        Utils::deserialize(Parallel::recv(processor + 1, Parallel::COMMAND_SYNC_VALUES), cell->getValues());
                    }
                }
                if (processor > 0) {
                    for (auto& cell : recvPrevCells) {
                        Utils::deserialize(Parallel::recv(processor - 1, Parallel::COMMAND_SYNC_VALUES), cell->getValues());
                    }
                    for (auto& cell : sendPrevCells) {
                        Parallel::send(Utils::serialize(cell->getValues()), processor - 1, Parallel::COMMAND_SYNC_VALUES);
                    }
                }
            } else {
                if (processor > 0) {
                    for (auto& cell : recvPrevCells) {
                        Utils::deserialize(Parallel::recv(processor - 1, Parallel::COMMAND_SYNC_VALUES), cell->getValues());
                    }
                    for (auto& cell : sendPrevCells) {
                        Parallel::send(Utils::serialize(cell->getValues()), processor - 1, Parallel::COMMAND_SYNC_VALUES);
                    }
                }
                if (processor < size - 1) {
                    for (auto& cell : sendNextCells) {
                        Parallel::send(Utils::serialize(cell->getValues()), processor + 1, Parallel::COMMAND_SYNC_VALUES);
                    }
                    for (auto& cell : recvNextCells) {
                        Utils::deserialize(Parallel::recv(processor + 1, Parallel::COMMAND_SYNC_VALUES), cell->getValues());
                    }
                }
            }
            break;
    }
}

Grid<CellParameters>* GridMaker::uniteGrids(const std::vector<Grid<CellParameters>*>& grids) {
    Vector2u size{0, grids[0]->getSize().y()};
    for (unsigned int i = 0; i < grids.size(); i++) {
        size.x() += grids[i]->getSize().x();
        if (i > 0) {
            size.x() -= 1;

        }
        if (i < grids.size() - 1) {
            size.x() -= 2;
        }

        if (i > 0) {
            if (grids[i]->get(1, 0) == nullptr) {
                size.x() -= 1;
            }
        }
    }

    auto* unitedGrid = new Grid<CellParameters>(size);

    unsigned int shiftX = 0;
    for (unsigned int i = 0; i < grids.size(); i++) {
        unsigned int startX = 0;
        if (i > 0) {
            startX += 1;
        }
        unsigned int endX = grids[i]->getSize().x();
        if (i < grids.size() - 1) {
            endX -= 2;
        }

        if (i > 0) {
            if (grids[i]->get(1, 0) == nullptr) {
                shiftX -= 1;
            }
        }

        for (unsigned int x = startX; x < endX; x++) {
            for (unsigned int y = 0; y < grids[i]->getSize().y(); y++) {
                auto* params = grids[i]->get(x, y);
                if (params != nullptr) {
                    unitedGrid->set({x + shiftX - startX, y}, params);
                }
            }
        }

        shiftX += grids[i]->getSize().x();
        if (i > 0) {
            shiftX -= 1;
        }
        if (i < grids.size() - 1) {
            shiftX -= 2;
        }
    }

    return unitedGrid;
}

void GridMaker::updateTimestep() {
    if (Parallel::isUsingMPI() == true && Parallel::getSize() > 1) {
        if (Parallel::isMaster() == true) {
            double minStep = 0.4 / 5;
            double timestep = 0.95 * 1.0 * minStep / Config::getInstance()->getImpulse()->getMaxImpulse();
            Config::getInstance()->setTimestep(timestep);

            // Send to other processes
            for (int processor = 1; processor < Parallel::getSize(); processor++) {
                Parallel::send(Utils::serialize(timestep), processor, Parallel::COMMAND_GRID_TIMESTEP);
            }
        } else {
            double timestep = 0.0;
            Utils::deserialize(Parallel::recv(0, Parallel::COMMAND_GRID_TIMESTEP), timestep);
            Config::getInstance()->setTimestep(timestep);
        }
    } else {
        double minStep = 0.4 / 7;
        double timestep = 0.9 * 1.0 * minStep / Config::getInstance()->getImpulse()->getMaxImpulse();
        Config::getInstance()->setTimestep(timestep);
    }
}