#include <chrono>

#include "Solver.h"
#include "grid/GridMaker.h"
#include "grid/Cell.h"
#include "parameters/BetaChain.h"
#include "integral/ci.hpp"
#include "ResultsFormatter.h"

Solver::Solver(const std::string& meshfile) {
    _config = Config::getInstance();
    _writer = new ResultsFormatter();
    _maker = new GridMaker(meshfile);
}

void Solver::init() {

    // init cell grid
    _grid = _maker->makeGrid();
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
            _maker->syncGrid(_grid);
        }

        // transfer
        _grid->computeTransfer();

        // integral
        if (_config->isUseIntegral()) {

            // sync grid
            if (Parallel::isUsingMPI() == true && Parallel::getSize() > 1) {
                _maker->syncGrid(_grid);
            }

            int iGasesNum = _config->getGasesCount();
            if (iGasesNum == 1) {
                _grid->computeIntegral(0, 0);
            } else if (iGasesNum == 2) {
                _grid->computeIntegral(0, 0);
                _grid->computeIntegral(0, 1);
            } else if (iGasesNum >= 3) {
                _grid->computeIntegral(0, 0);
                _grid->computeIntegral(0, 1);
                _grid->computeIntegral(0, 2);
            }
        }

        // beta decay
        if (_config->isUseBetaChains()) {

            // sync grid
            if (Parallel::isUsingMPI() == true && Parallel::getSize() > 1) {
                _maker->syncGrid(_grid);
            }

            for (int i = 0; i < _config->getBetaChainsCount(); i++) {
                auto& item = _config->getBetaChains()[i];
                _grid->computeBetaDecay(item.iGasIndex1, item.iGasIndex2, item.dLambda1);
                _grid->computeBetaDecay(item.iGasIndex2, item.iGasIndex3, item.dLambda2);
            }
        }

//        if (iteration % _config->getOutEach() == 0) {
//
//            // create result params grid
//            Grid<CellParameters>* rpGrid = new Grid<CellParameters>(_grid->getSize());
//            for (unsigned int i = 0; i < _grid->getCount(); i++) {
//                auto* cell = _grid->getByIndex(i);
//                if (cell != nullptr && cell->getData()->isProcessing() == true) {
//                    auto& params = cell->getResultParams();
//                    rpGrid->setByIndex(i, &params);
//                }
//            }
//            if (Parallel::isUsingMPI() == true && Parallel::getSize() > 1) {
//                if (Parallel::isMaster() == true) {
//                    std::vector<Grid<CellParameters>*> rpGrids;
//                    rpGrids.push_back(rpGrid);
//
//                    // receive params from master, then unite grids
//                    for (int processor = 1; processor < Parallel::getSize(); processor++) {
//                        Grid<CellParameters>* rpGridFromSlave = nullptr;
//                        Utils::deserialize(Parallel::recv(processor, Parallel::COMMAND_RESULT_PARAMS), rpGridFromSlave);
//                        rpGrids.push_back(rpGridFromSlave);
//                    }
//
//                    // unite received grids
//                    rpGrid = _maker->uniteGrids(rpGrids);
//                    for (unsigned int i = 0; i < rpGrids.size(); i++) {
//                        delete (rpGrids[i]);
//                    }
//
//                    _writer->writeAll(rpGrid, iteration);
//                    delete (rpGrid);
//                } else {
//
//                    // send params to master
//                    Parallel::send(Utils::serialize(rpGrid), 0, Parallel::COMMAND_RESULT_PARAMS);
//                    delete (rpGrid);
//                }
//            } else {
//                _writer->writeAll(rpGrid, iteration);
//                delete (rpGrid);
//            }
//        }

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
