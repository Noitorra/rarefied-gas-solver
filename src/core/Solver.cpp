#include "Solver.h"
#include "grid/NormalCell.h"
#include "parameters/BetaChain.h"
#include "integral/ci.hpp"
#include "utilities/Parallel.h"
#include "utilities/Utils.h"
#include "mesh/MeshParser.h"
#include "ResultsFormatter.h"

#include <chrono>

Solver::Solver(const std::string& meshfile) {
    _meshfile = meshfile;
    _config = Config::getInstance();
    _formatter = new ResultsFormatter();
    _wholeMesh = nullptr;
}

void Solver::init() {
    Mesh* mesh = nullptr;

    if (Parallel::isUsingMPI() == true && Parallel::getSize() > 1) {
        if (Parallel::isMaster() == true) {

            // load mesh
            _wholeMesh = MeshParser::getInstance().loadMesh(_meshfile);
            _wholeMesh->init();

            // split mesh
            auto meshes = _wholeMesh->split();

            // select self mesh
            mesh = meshes[0];

            // send to other processes
            for (int processor = 1; processor < Parallel::getSize(); processor++) {
                Parallel::send(Utils::serialize(meshes[processor]), processor, Parallel::COMMAND_MESH);
//                delete(meshes[processor]);
            }
        } else {
            // get mesh from master process
            Utils::deserialize(Parallel::recv(0, Parallel::COMMAND_MESH), mesh);
        }
    } else {

        // load mesh
        _wholeMesh = MeshParser::getInstance().loadMesh(_meshfile);
        _wholeMesh->init();
        mesh = _wholeMesh;
    }

    // init all
    _grid = new Grid(mesh);
    _grid->init();
}

void Solver::run() {
    if (_config->isUseIntegral()) {
        ci::Potential* potential = new ci::HSPotential;
        ci::init(potential, ci::NO_SYMM);
    }

    if (Parallel::isMaster() == true) {
        std::cout << std::endl;
    }
    unsigned int maxIteration = _config->getMaxIteration();
    for (unsigned int iteration = 0; iteration < maxIteration; iteration++) {

        // sync grid
        if (Parallel::isUsingMPI() == true && Parallel::getSize() > 1) {
            _grid->sync();
        }

        // transfer
        _grid->computeTransfer();

        // integral
        if (_config->isUseIntegral()) {
            int gasesSize = _config->getGases().size();
            if (gasesSize == 1) {
                _grid->computeIntegral(0, 0);
            } else if (gasesSize == 2) {
                _grid->computeIntegral(0, 0);
                _grid->computeIntegral(0, 1);
            } else if (gasesSize >= 3) {
                _grid->computeIntegral(0, 0);
                _grid->computeIntegral(0, 1);
                _grid->computeIntegral(0, 2);
            }
        }

        // beta decay
        if (_config->isUseBetaChains()) {
            const auto& betaChains = _config->getBetaChains();
            for (const auto& betaChain : betaChains) {
                _grid->computeBetaDecay(betaChain.getGasIndex1(), betaChain.getGasIndex2(), betaChain.getLambda1());
                _grid->computeBetaDecay(betaChain.getGasIndex2(), betaChain.getGasIndex3(), betaChain.getLambda2());
            }
        }

        // check grid
        _grid->check();

        if (iteration % _config->getOutEach() == 0) {
            std::vector<CellResults*> results;
            for (const auto& cell : _grid->getCells()) {
                if (cell->getType() == NormalCell::Type::NORMAL) {
                    auto normalCell = dynamic_cast<NormalCell*>(cell.get());
                    results.push_back(normalCell->getResults());
                }
            }

            if (Parallel::isUsingMPI() == true && Parallel::getSize() > 1) {
                if (Parallel::isMaster() == true) {

                    // receive params from master, then unite grids
                    for (int processor = 1; processor < Parallel::getSize(); processor++) {
                        std::vector<CellResults*> resultsBuffer;
                        Utils::deserialize(Parallel::recv(processor, Parallel::COMMAND_RESULT_PARAMS), resultsBuffer);
                        for (auto tempResults : resultsBuffer) {
                            results.push_back(tempResults);
                        }
                    }

                    _formatter->writeAll(_wholeMesh, results, iteration);
                } else {

                    // send params to master
                    Parallel::send(Utils::serialize(results), 0, Parallel::COMMAND_RESULT_PARAMS);
                }
            } else {
                _formatter->writeAll(_wholeMesh, results, iteration);
            }
        }

        if (Parallel::isMaster() == true) {
            auto percent = (unsigned int) (1.0 * (iteration + 1) / maxIteration * 100);

            std::cout << '\r';
            std::cout << "[" << std::string(percent, '#') << std::string(100 - percent, '-') << "] ";
            std::cout << "[" << iteration << "/" << maxIteration << "] ";
            std::cout << percent << "%";
            std::cout.flush();
        }
    }
    if (Parallel::isMaster() == true) {
        std::cout << std::endl << "Done" << std::endl;
    }
}
