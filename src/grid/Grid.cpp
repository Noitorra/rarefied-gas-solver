#include "Grid.h"
#include "NormalCell.h"
#include "BorderCell.h"
#include "ParallelCell.h"
#include "CellConnection.h"
#include "mesh/Mesh.h"
#include "parameters/Gas.h"
#include "parameters/Impulse.h"
#include "utilities/Parallel.h"
#include "integral/ci.hpp"
#include "utilities/Utils.h"

#include <unordered_map>
#include <map>

Grid::Grid(Mesh* mesh) : _mesh(mesh) {

    // element -> cell
    for (const auto& element : _mesh->getElements()) {
        if (element->getType() == Element::Type::TRIANGLE) {
            auto cell = new NormalCell(element->getId(), element->getVolume());
            cell->getParams().set(0, 1.0, 1.0, 1.0);
            addCell(cell);
        }
    }

    bool isUsingParallel = Parallel::isUsingMPI() == true && Parallel::getSize() > 1;

    // side elements -> cell connections
    for (const auto& element : _mesh->getElements()) {
        if (element->getType() == Element::Type::TRIANGLE) {
            auto cell = getCellById(element->getId());

            for (const auto& sideElement : element->getSideElements()) {
                BaseCell* otherCell = nullptr;

                auto neighborId = sideElement->getNeighborId();
                if (isUsingParallel == false) {
                    if (neighborId < 0) {
                        neighborId = -neighborId;
                    }
                }
                if (neighborId > 0) {
                    otherCell = getCellById(neighborId);
                } else if (neighborId < 0) {
                    otherCell = getCellById(neighborId);
                    if (otherCell == nullptr) {
                        auto parallelCell = new ParallelCell(-neighborId, sideElement->getNeighborProcessId());
                        addCell(parallelCell);

                        otherCell = parallelCell;
                    }
                    auto parallelConnection = new CellConnection(otherCell, cell, sideElement->getElement()->getVolume(), -sideElement->getNormal());
                    otherCell->addConnection(parallelConnection);
                } else {
                    auto borderCell = new BorderCell(BorderCell::BorderType::DIFFUSE);
                    borderCell->getBoundaryParams().set(0, 1.0, 1.0, 0.5);
                    addCell(borderCell);

                    auto borderConnection = new CellConnection(borderCell, cell, sideElement->getElement()->getVolume(), -sideElement->getNormal());
                    borderCell->addConnection(borderConnection);

                    otherCell = borderCell;
                }

                if (otherCell != nullptr) {
                    auto connection = new CellConnection(cell, otherCell, sideElement->getElement()->getVolume(), sideElement->getNormal());
                    cell->addConnection(connection);
                }
            }
        }
    }
}

Mesh* Grid::getMesh() const {
    return _mesh;
}

void Grid::addCell(BaseCell* cell) {
    if (cell->getId() != 0) {
        if (_cellsMap[cell->getId()] == nullptr) {
            _cells.emplace_back(cell);
            _cellsMap[cell->getId()] = _cells.back().get();
        }
    } else {
        _cells.emplace_back(cell);
    }
}

const std::vector<std::shared_ptr<BaseCell>>& Grid::getCells() const {
    return _cells;
}

BaseCell* Grid::getCellById(int id) {
    return _cellsMap[id];
}

void Grid::init() {
    for (const auto& cell : _cells) {
        cell->init();
    }

    double minStep = std::numeric_limits<double>::max();
    for (const auto& cell : _cells) {
        if (cell->getType() == BaseCell::Type::NORMAL) {
            auto normalCell = dynamic_cast<NormalCell*>(cell.get());

            double maxSquare = 0.0;
            for (const auto& connection : normalCell->getConnections()) {
                maxSquare = std::max(connection->getSquare(), maxSquare);
            }
            double step = normalCell->getVolume() / maxSquare;
            minStep = std::min(minStep, step);
        }
    }

    auto config = Config::getInstance();
    double timestep = 0.9 * minStep / config->getImpulse()->getMaxImpulse();

    if (Parallel::isUsingMPI() == true && Parallel::getSize() > 1) {
        if (Parallel::isMaster()) {
            for (auto rank = 1; rank < Parallel::getSize(); rank++) {
                double otherTimestep = 0.0;
                Utils::deserialize(Parallel::recv(rank, Parallel::COMMAND_TIMESTEP), otherTimestep);
                timestep = std::min(timestep, otherTimestep);
            }

            for (auto rank = 1; rank < Parallel::getSize(); rank++) {
                Parallel::send(Utils::serialize(timestep), rank, Parallel::COMMAND_TIMESTEP);
            }
        } else {
            Parallel::send(Utils::serialize(timestep), 0, Parallel::COMMAND_TIMESTEP);
            Utils::deserialize(Parallel::recv(0, Parallel::COMMAND_TIMESTEP), timestep);
        }
    }

    config->setTimestep(timestep);
}

void Grid::computeTransfer() {

    // first go for border cells
    for (const auto& cell : _cells) {
        if (cell->getType() == BaseCell::Type::BORDER) {
            cell->computeTransfer();
        }
    }

    // then go for normal cells
    for (const auto& cell : _cells) {
        if (cell->getType() == BaseCell::Type::NORMAL) {
            cell->computeTransfer();
        }
    }

    // move changes from next step to current step
    for (const auto& cell : _cells) {
        if (cell->getType() == BaseCell::Type::NORMAL) {
            auto normalCell = dynamic_cast<NormalCell*>(cell.get());
            normalCell->swapValues();
        }
    }
}

void Grid::computeIntegral(unsigned int gi1, unsigned int gi2) {
    auto impulse = Config::getInstance()->getImpulse();
    const auto& gases = Config::getInstance()->getGases();
    double timestep = Config::getInstance()->getTimestep();

    ci::Particle cParticle{};
    cParticle.d = 1.;

    ci::gen(timestep, 50000,
            impulse->getResolution() / 2, impulse->getResolution() / 2,
            impulse->getXYZ2I(), impulse->getXYZ2I(),
            impulse->getMaxImpulse() / impulse->getResolution() * 2,
            gases[gi1].getMass(), gases[gi2].getMass(),
            cParticle, cParticle);

    for (const auto& cell : _cells) {
        cell->computeIntegral(gi1, gi2);
    }
}

void Grid::computeBetaDecay(unsigned int gi0, unsigned int gi1, double lambda) {
    for (const auto& cell : _cells) {
        cell->computeBetaDecay(gi0, gi1, lambda);
    }
}

void Grid::check() {
    for (const auto& cell : _cells) {
        cell->check();
    }
}

void Grid::sync() {
    std::unordered_map<int, std::vector<int>> sendSyncIdsMap;
    std::unordered_map<int, std::vector<int>> recvSyncIdsMap;

    // fill map
    for (const auto& cell : _cells) {
        if (cell->getType() == BaseCell::Type::PARALLEL) {
            auto parallelCell = dynamic_cast<ParallelCell*>(cell.get());

            // create vectors for data
            auto syncProcessId = parallelCell->getSyncProcessId();
            if (sendSyncIdsMap.count(syncProcessId) == 0) {
                sendSyncIdsMap.emplace(syncProcessId, std::vector<int>());
            }
            if (recvSyncIdsMap.count(syncProcessId) == 0) {
                recvSyncIdsMap.emplace(syncProcessId, std::vector<int>());
            }

            // add send elements
            auto& sendSyncIds = sendSyncIdsMap[syncProcessId];
            const auto& cellSendSyncIds = parallelCell->getSendSyncIds();
            sendSyncIds.insert(sendSyncIds.end(), cellSendSyncIds.begin(), cellSendSyncIds.end());

            // add recv element
            auto& recvSyncIds = recvSyncIdsMap[syncProcessId];
            recvSyncIds.insert(recvSyncIds.end(), parallelCell->getRecvSyncId());
        }
    }

    // sort all
    for (const auto& pair : sendSyncIdsMap) {
        auto& sendSyncIds = sendSyncIdsMap.at(pair.first);
        std::sort(sendSyncIds.begin(), sendSyncIds.end());
    }
    for (const auto& pair : recvSyncIdsMap) {
        auto& recvSyncIds = recvSyncIdsMap.at(pair.first);
        std::sort(recvSyncIds.begin(), recvSyncIds.end());
    }

//    for (const auto& pair : sendSyncIdsMap) {
//        std::cout << "Send to process: " << pair.first << std::endl;
//        for (const auto& sendSyncId : pair.second) {
//            std::cout << sendSyncId << " ";
//        }
//        std::cout << std::endl;
//    }
//
//    for (const auto& pair : recvSyncIdsMap) {
//        std::cout << "Recv from process: " << pair.first << std::endl;
//        for (const auto& recvSyncId : pair.second) {
//            std::cout << recvSyncId << " ";
//        }
//        std::cout << std::endl;
//    }

    // send and recv
    for (auto rank = 0; rank < Parallel::getSize(); rank++) {
        if (rank == Parallel::getRank()) {
            // recv
            for (auto otherRank = 0; otherRank < Parallel::getSize(); otherRank++) {
                if (otherRank != rank) {
                    if (recvSyncIdsMap.count(otherRank) != 0) {
                        const auto& recvSyncIds = recvSyncIdsMap[otherRank];
                        for (auto recvSyncId : recvSyncIds) {
                            auto cell = getCellById(-recvSyncId);
                            Utils::deserialize(Parallel::recv(otherRank, Parallel::COMMAND_SYNC_VALUES), cell->getValues());
                        }
                    }
                }
            }
        } else {
            // send to rank process
            if (sendSyncIdsMap.count(rank) != 0) {
                const auto& sendSyncIds = sendSyncIdsMap[rank];
                for (auto sendSyncId : sendSyncIds) {
                    auto cell = getCellById(sendSyncId);
                    Parallel::send(Utils::serialize(cell->getValues()), rank, Parallel::COMMAND_SYNC_VALUES);
                }
            }
        }
    }
}
