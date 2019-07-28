#include "Solver.h"
#include "grid/NormalCell.h"
#include "parameters/BetaChain.h"
#include "integral/ci.hpp"
#include "utilities/Parallel.h"
#include "utilities/Utils.h"
#include "utilities/SerializationUtils.h"
#include "mesh/MeshParser.h"
#include "ResultsFormatter.h"
#include "KeyboardManager.h"

#include <chrono>
#include <stdexcept>

Solver::Solver() {
    _config = Config::getInstance();
    _formatter = new ResultsFormatter();
    _keyboard = KeyboardManager::getInstance();
}

void Solver::init() {
    Mesh* mesh = nullptr;

    if (Parallel::isSingle() == false) {
        if (Parallel::isMaster() == true) {

            // load mesh
            mesh = MeshParser::getInstance().loadMesh(_config->getMeshFilename(), _config->getMeshUnits());
            mesh->init();

            // send to other processes
            for (int processor = 1; processor < Parallel::getSize(); processor++) {
                Parallel::send(SerializationUtils::serialize(mesh), processor, Parallel::COMMAND_MESH);
            }
        } else {

            // get mesh from master process
            SerializationUtils::deserialize(Parallel::recv(0, Parallel::COMMAND_MESH), mesh);
            mesh->resetMaps();
        }
    } else {

        // load mesh
        mesh = MeshParser::getInstance().loadMesh(_config->getMeshFilename(), _config->getMeshUnits());
        mesh->init();
    }

    // init all
    _grid = new Grid(mesh);
    _grid->init();

    // initiate integral
    if (_config->isUsingIntegral()) {
        ci::Potential* potential = new ci::HSPotential;
        ci::init(potential, ci::NO_SYMM);
    }
}

void Solver::run() {

    // start keyboard listener
    if (Parallel::isMaster() == true) {
        if (_keyboard->isAvailable()) {
            _keyboard->start();
        }
        std::cout << std::endl;
    }

    // write initial results
    writeResults(0);

    unsigned int prevPercent = 0;
    unsigned int maxIterations = _config->getMaxIterations();
    for (unsigned int iteration = 1; iteration <= maxIterations; iteration++) {

        // transfer
        _grid->computeTransfer();

        // integral
        if (_config->isUsingIntegral()) {
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
        if (_config->isUsingBetaDecay()) {
            const auto& betaChains = _config->getBetaChains();
            for (const auto& betaChain : betaChains) {
                _grid->computeBetaDecay(betaChain.getGasIndex1(), betaChain.getGasIndex2(), betaChain.getLambda1());
                _grid->computeBetaDecay(betaChain.getGasIndex2(), betaChain.getGasIndex3(), betaChain.getLambda2());
            }
        }

        // transfer
        _grid->computeTransfer();

        // check grid
        _grid->check();

        // print out results
        if (iteration % _config->getOutEachIteration() == 0) {
            writeResults(iteration);
        }

        if (Parallel::isMaster() == true) {
            bool isPrintingProgress = true;
            if (_keyboard->isAvailable()) {
                isPrintingProgress = _keyboard->isWaitingCommand() == false;
            }
            if (isPrintingProgress) {
                auto percent = (unsigned int) (1.0 * iteration / maxIterations * 100);
                if (percent > prevPercent) {
                    prevPercent = percent;

                    std::cout << '\r';
                    std::cout << "[" << std::string(percent, '#') << std::string(100 - percent, '-') << "] ";
                    std::cout << "[" << iteration << "/" << maxIterations << "] ";
                    std::cout << percent << "%";
                    std::cout.flush();
                }
            }
            if (_keyboard->isAvailable() && _keyboard->isStop()) {
                throw std::runtime_error("stop signal");
            }
        }
    }
    if (Parallel::isMaster() == true) {
        std::cout << std::endl << "Done" << std::endl;
    }
}

void Solver::writeResults(int iteration) {
    std::vector<CellResults*> results;
    for (const auto& cell : _grid->getCells()) {
        if (cell->getType() == NormalCell::Type::NORMAL) {
            auto normalCell = dynamic_cast<NormalCell*>(cell.get());
            results.push_back(normalCell->getResults());
        }
    }

    if (Parallel::isSingle() == false) {
        if (Parallel::isMaster() == true) {

            // receive params from slaves, then unite grids
            for (int processor = 1; processor < Parallel::getSize(); processor++) {
                std::vector<CellResults*> resultsBuffer;
                SerializationUtils::deserialize(Parallel::recv(processor, Parallel::COMMAND_RESULT_PARAMS), resultsBuffer);
                for (auto tempResults : resultsBuffer) {
                    results.push_back(tempResults);
                }
            }

            _formatter->writeAll(iteration, _grid->getMesh(), results);
        } else {

            // send params to master
            Parallel::send(SerializationUtils::serialize(results), 0, Parallel::COMMAND_RESULT_PARAMS);
        }
    } else {
        _formatter->writeAll(iteration, _grid->getMesh(), results);
    }
}
