#include "Grid.h"
#include "NormalCell.h"
#include "BorderCell.h"
#include "ParallelCell.h"
#include "CellConnection.h"
#include "mesh/Mesh.h"
#include "parameters/Gas.h"
#include "parameters/ImpulseSphere.h"
#include "parameters/InitialParameters.h"
#include "parameters/BoundaryParameters.h"
#include "utilities/Parallel.h"
#include "utilities/SerializationUtils.h"
#include "utilities/Normalizer.h"
#include "integral/ci.hpp"

#include <unordered_map>
#include <map>

Grid::Grid(Mesh* mesh) : _mesh(mesh) {
    auto config = Config::getInstance();
    auto normalizer = config->getNormalizer();
    const auto& initialParameters = config->getInitialParameters();
    const auto& boundaryParameters = config->getBoundaryParameters();

    // create normal cell for "Main" elements for current process
    for (const auto& element : _mesh->getElements()) {
        if (element->isMain() == true) {
            if (Parallel::isSingle() == false && element->getProcessId() != Parallel::getRank()) {
                continue;
            }

            // create normal cell
            auto cell = new NormalCell(element->getId(), element->getVolume());
            addCell(cell);

            // set initial params by physical group
            for (const auto& param : initialParameters) {
                if (param.getGroup() == element->getGroup()) {
                    for (auto i = 0; i < param.getPressure().size(); i++) {
                        auto pressure = normalizer->normalize(param.getPressure()[i], Normalizer::Type::PRESSURE);
                        cell->getParams().setPressure(i, pressure);
                    }
                    for (auto i = 0; i < param.getTemperature().size(); i++) {
                        auto temperature = normalizer->normalize(param.getTemperature()[i], Normalizer::Type::TEMPERATURE);
                        cell->getParams().setTemp(i, temperature);
                    }
                }
            }
        }
    }

    std::vector<BaseCell*> cells;
    for (const auto& cell : _cells) {
        cells.push_back(cell.get());
    }

    // create cell connections
    for (const auto& cell : cells) {
        auto element = _mesh->getElement(cell->getId());
        for (const auto& sideElement : element->getSideElements()) {
            auto neighborElement = _mesh->getElement(sideElement->getNeighborId());

            if (neighborElement->isMain()) {
                if (Parallel::isSingle() || neighborElement->getProcessId() == Parallel::getRank()) {

                    // create connection for cell with other normal cell
                    auto neighborCell = getCellById(neighborElement->getId());
                    auto connection = new CellConnection(cell, neighborCell, sideElement->getElement()->getVolume(), sideElement->getNormal());
                    cell->addConnection(connection);
                } else {

                    // create or get parallel cell
                    BaseCell* parallelCell = getCellById(-neighborElement->getId());
                    if (parallelCell == nullptr) {
                        parallelCell = new ParallelCell(-neighborElement->getId(), neighborElement->getId(), neighborElement->getProcessId());
                        addCell(parallelCell);
                    }

                    // create connection for parallel cell
                    auto parallelConnection = new CellConnection(parallelCell, cell, sideElement->getElement()->getVolume(), -sideElement->getNormal());
                    parallelCell->addConnection(parallelConnection);

                    // create connection for cell
                    auto connection = new CellConnection(cell, parallelCell, sideElement->getElement()->getVolume(), sideElement->getNormal());
                    cell->addConnection(connection);
                }
            } else if (neighborElement->isBorder()) {

                // create border cell
                auto borderCell = new BorderCell(neighborElement->getId());
                addCell(borderCell);

                // set boundary params by physical group
                for (const auto& param : boundaryParameters) {
                    if (param.getGroup() == neighborElement->getGroup()) {
                        const auto& type = param.getType();
                        if (type == "Diffuse") {
                            borderCell->setBorderType(BorderCell::BorderType::DIFFUSE);
                        } else if (type == "Mirror") {
                            borderCell->setBorderType(BorderCell::BorderType::MIRROR);
                        }
                        for (auto i = 0; i < param.getTemperature().size(); i++) {
                            auto temperature = normalizer->normalize(param.getTemperature()[i], Normalizer::Type::TEMPERATURE);
                            borderCell->getBoundaryParams().setTemp(i, temperature);
                        }
                    }
                }

                // create connection for border cell
                auto borderConnection = new CellConnection(borderCell, cell, sideElement->getElement()->getVolume(), -sideElement->getNormal());
                borderCell->addConnection(borderConnection);

                // create connection for cell
                auto connection = new CellConnection(cell, borderCell, sideElement->getElement()->getVolume(), sideElement->getNormal());
                cell->addConnection(connection);
            } else {
                throw std::runtime_error("wrong neighbor element");
            }
        }
    }
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
    double timestep = 0.95 * minStep / config->getImpulseSphere()->getMaxImpulse();

    if (Parallel::isSingle() == false) {
        if (Parallel::isMaster() == true) {
            for (auto rank = 1; rank < Parallel::getSize(); rank++) {
                double otherTimestep = 0.0;
                SerializationUtils::deserialize(Parallel::recv(rank, Parallel::COMMAND_TIMESTEP), otherTimestep);
                timestep = std::min(timestep, otherTimestep);
            }

            for (auto rank = 1; rank < Parallel::getSize(); rank++) {
                Parallel::send(SerializationUtils::serialize(timestep), rank, Parallel::COMMAND_TIMESTEP);
            }
        } else {
            Parallel::send(SerializationUtils::serialize(timestep), 0, Parallel::COMMAND_TIMESTEP);
            SerializationUtils::deserialize(Parallel::recv(0, Parallel::COMMAND_TIMESTEP), timestep);
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
    auto impulse = Config::getInstance()->getImpulseSphere();
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
        std::vector<int>& sendSyncIds = sendSyncIdsMap.at(pair.first);
        std::sort(sendSyncIds.begin(), sendSyncIds.end());
        sendSyncIds.erase(std::unique(sendSyncIds.begin(), sendSyncIds.end()), sendSyncIds.end());
    }
    for (const auto& pair : recvSyncIdsMap) {
        std::vector<int>& recvSyncIds = recvSyncIdsMap.at(pair.first);
        std::sort(recvSyncIds.begin(), recvSyncIds.end());
        recvSyncIds.erase(std::unique(recvSyncIds.begin(), recvSyncIds.end()), recvSyncIds.end());
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
                            SerializationUtils::deserialize(Parallel::recv(otherRank, Parallel::COMMAND_SYNC_VALUES), cell->getValues());
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
                    Parallel::send(SerializationUtils::serialize(cell->getValues()), rank, Parallel::COMMAND_SYNC_VALUES);
                }
            }
        }
    }
}

Mesh* Grid::getMesh() const {
    return _mesh;
}

void Grid::addCell(BaseCell* cell) {
    if (_cellsMap[cell->getId()] == nullptr) {
        _cellsMap[cell->getId()] = cell;
        _cells.emplace_back(cell);
    }
}

BaseCell* Grid::getCellById(int id) {
    return _cellsMap[id];
}

const std::vector<std::shared_ptr<BaseCell>>& Grid::getCells() const {
    return _cells;
}


