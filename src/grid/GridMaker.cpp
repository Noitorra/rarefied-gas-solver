#include "GridMaker.h"
#include "CellData.h"
#include "parameters/Impulse.h"
#include "mesh/Mesh.h"
#include "mesh/MeshParser.h"

/*
 * How to make a grid? Easy!
 * - setup physical params: size, macro values, etc
 * - normalize data
 *
 * MPI workflow:
 * - main process load all configs, splits them for each process, add's special cells
 * - slave process just loads configs and that's it...
 */
Grid* GridMaker::makeGrid() {
    Mesh* mesh = nullptr;

    if (Parallel::isUsingMPI() == true && Parallel::getSize() > 1) {
        if (Parallel::isMaster() == true) {

            // load mesh
            mesh = MeshParser::getInstance().loadMesh(_meshfile);
            std::cout << "Count Nodes = " << mesh->getNodesCount() << std::endl;
            std::cout << "Count Elements = " << mesh->getElementsCount() << std::endl;

            // update config time
            updateTimestep();

            // select self mesh
            mesh = nullptr;

            // send to other processes
//            for (int processor = 1; processor < Parallel::getSize(); processor++) {
//                Parallel::send(Utils::serialize(grids[processor]), processor, Parallel::COMMAND_GRID);
//                delete(grids[processor]);
//            }
        } else {
            // update config time
            updateTimestep();

            // get mesh from master process
//            Utils::deserialize(Parallel::recv(0, Parallel::COMMAND_GRID), grid);
        }
    } else {
        // update config time
        updateTimestep();
    }

    return new Grid(mesh);
}

std::vector<Grid*> GridMaker::divideGrid(Grid* grid, unsigned int numGrids) {
    return {};
}

void GridMaker::syncGrid(Grid* grid) {

//    // need vectors of ids
//    std::vector<int> sendNextIds;
//    std::vector<int> sendPrevIds;
//    std::vector<int> recvNextIds;
//    std::vector<int> recvPrevIds;
//
//    // need vectors of cells
//    std::vector<Cell*> sendNextCells;
//    std::vector<Cell*> sendPrevCells;
//    std::vector<Cell*> recvNextCells;
//    std::vector<Cell*> recvPrevCells;
//
//    int processor = Parallel::getRank();
//    int size = Parallel::getSize();
//
//    // fill send vectors
//    for (auto& cell : grid->getValues()) {
//        if (cell != nullptr) {
//            if (cell->getData()->isSyncValues() == false) {
//                continue;
//            }
//            bool isSyncAvaliableOnAxis = true;
//            for (auto axis : syncAxis) {
//                if (cell->getData()->isSyncAvaliable(axis) == false) {
//                    isSyncAvaliableOnAxis = false;
//                    break;
//                }
//            }
//            if (isSyncAvaliableOnAxis == false) {
//                continue;
//            }
//
//            int originalProcessor = cell->getData()->getSyncProcessorRank();
//            if (originalProcessor > processor) {
//                sendNextIds.push_back(cell->getData()->getSyncId());
//                recvNextCells.push_back(cell);
//            } else if (originalProcessor < processor) {
//                sendPrevIds.push_back(cell->getData()->getSyncId());
//                recvPrevCells.push_back(cell);
//            }
//        }
//    }
//
//    // parallel exchange for ids
//    if (processor % 2 == 0) {
//        if (processor < size - 1) {
//            Parallel::send(Utils::serialize(sendNextIds), processor + 1, Parallel::COMMAND_SYNC_IDS);
//            Utils::deserialize(Parallel::recv(processor + 1, Parallel::COMMAND_SYNC_IDS), recvNextIds);
//        }
//        if (processor > 0) {
//            Utils::deserialize(Parallel::recv(processor - 1, Parallel::COMMAND_SYNC_IDS), recvPrevIds);
//            Parallel::send(Utils::serialize(sendPrevIds), processor - 1, Parallel::COMMAND_SYNC_IDS);
//        }
//    } else {
//        if (processor > 0) {
//            Utils::deserialize(Parallel::recv(processor - 1, Parallel::COMMAND_SYNC_IDS), recvPrevIds);
//            Parallel::send(Utils::serialize(sendPrevIds), processor - 1, Parallel::COMMAND_SYNC_IDS);
//        }
//        if (processor < size - 1) {
//            Parallel::send(Utils::serialize(sendNextIds), processor + 1, Parallel::COMMAND_SYNC_IDS);
//            Utils::deserialize(Parallel::recv(processor + 1, Parallel::COMMAND_SYNC_IDS), recvNextIds);
//        }
//    }
//
//    // fill recv vectors
//    for (auto& id : recvNextIds) {
//        for (auto& cell : grid->getValues()) {
//            if (cell != nullptr) {
//                if (id == cell->getData()->getId()) {
//                    sendNextCells.push_back(cell);
//                }
//            }
//        }
//    }
//    for (auto& id : recvPrevIds) {
//        for (auto& cell : grid->getValues()) {
//            if (cell != nullptr) {
//                if (id == cell->getData()->getId()) {
//                    sendPrevCells.push_back(cell);
//                }
//            }
//        }
//    }
//
//    // parallel exchange for normal values
//    if (processor % 2 == 0) {
//        if (processor < size - 1) {
//            for (auto& cell : sendNextCells) {
//                Parallel::send(Utils::serialize(cell->getValues()), processor + 1, Parallel::COMMAND_SYNC_VALUES);
//            }
//            for (auto& cell : recvNextCells) {
//                Utils::deserialize(Parallel::recv(processor + 1, Parallel::COMMAND_SYNC_VALUES), cell->getValues());
//            }
//        }
//        if (processor > 0) {
//            for (auto& cell : recvPrevCells) {
//                Utils::deserialize(Parallel::recv(processor - 1, Parallel::COMMAND_SYNC_VALUES), cell->getValues());
//            }
//            for (auto& cell : sendPrevCells) {
//                Parallel::send(Utils::serialize(cell->getValues()), processor - 1, Parallel::COMMAND_SYNC_VALUES);
//            }
//        }
//    } else {
//        if (processor > 0) {
//            for (auto& cell : recvPrevCells) {
//                Utils::deserialize(Parallel::recv(processor - 1, Parallel::COMMAND_SYNC_VALUES), cell->getValues());
//            }
//            for (auto& cell : sendPrevCells) {
//                Parallel::send(Utils::serialize(cell->getValues()), processor - 1, Parallel::COMMAND_SYNC_VALUES);
//            }
//        }
//        if (processor < size - 1) {
//            for (auto& cell : sendNextCells) {
//                Parallel::send(Utils::serialize(cell->getValues()), processor + 1, Parallel::COMMAND_SYNC_VALUES);
//            }
//            for (auto& cell : recvNextCells) {
//                Utils::deserialize(Parallel::recv(processor + 1, Parallel::COMMAND_SYNC_VALUES), cell->getValues());
//            }
//        }
//    }
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