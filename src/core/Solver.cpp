#include "Solver.h"

#include <chrono>
#include <grid/Cell.h>

#include "utilities/normalizer.h"
#include "parameters/gas.h"
#include "parameters/beta_chain.h"
#include "integral/ci.hpp"

void Solver::init(Grid<CellData>* grid) {
    _config = Config::getInstance();
    _impulse = _config->getImpulse();

    // make cell grid
    _grid = new Grid<Cell>(grid->getSize());
    for (unsigned int i = 0; i < grid->getCount(); i++) {
        CellData* data = grid->getByIndex(i);
        if (data != nullptr) {
            auto* cell = new Cell(data);
            cell->init();
            _grid->setByIndex(i, cell);
        }
    }

    // link
    for (unsigned int x = 0; x < _grid->getSize().x(); x++) {
        for (unsigned int y = 0; y < _grid->getSize().y(); y++) {
            Cell* cell = _grid->get(x, y);
            if (cell != nullptr) {
                Cell* leftCell = nullptr;
                Cell* rightCell = nullptr;
                Cell* topCell = nullptr;
                Cell* bottomCell = nullptr;

                if (x != 0) {
                    leftCell = _grid->get(x - 1, y);
                }
                if (x != _grid->getSize().x() - 1) {
                    rightCell = _grid->get(x + 1, y);
                }
                if (y != 0) {
                    bottomCell = _grid->get(x, y - 1);
                }
                if (y != _grid->getSize().y() - 1) {
                    topCell = _grid->get(x, y + 1);
                }

                cell->link(sep::Axis::X, leftCell, rightCell);
                cell->link(sep::Axis::Y, bottomCell, topCell);
            }
        }
    }
}

void Solver::run() {

    // Compute cell type for each axis
    initCellType(sep::X);
    initCellType(sep::Y);
    initCellType(sep::Z);

    //  m_pGridManager->PrintLinkage(sep::X);
    //  m_pGridManager->PrintLinkage(sep::Y);

    if (_config->isUseIntegral()) {
        ci::Potential* potential = new ci::HSPotential;
        ci::init(potential, ci::NO_SYMM);
    }

    auto startTimestamp = std::chrono::steady_clock::now();
    auto timestamp = startTimestamp;

    for (int it = 0; it < _config->getMaxIteration(); it++) {

        makeStep(sep::X);
        makeStep(sep::Y);
        makeStep(sep::Z);

        if (_config->isUseIntegral()) {
            int iGasesNum = _config->getGasesCount();
            double dTimestep = _config->getTimestep();

            if (iGasesNum == 1) {
                makeIntegral(0, 0, dTimestep);
            } else if (iGasesNum == 2) {
                makeIntegral(0, 0, dTimestep);
                makeIntegral(0, 1, dTimestep);
            } else if (iGasesNum >= 3) {
                makeIntegral(0, 0, dTimestep);
                makeIntegral(0, 1, dTimestep);
                makeIntegral(0, 2, dTimestep);
            }
        }

        if (_config->isUseBetaChains()) {
            for (int i = 0; i < _config->getBetaChainsNum(); i++) {
                auto& item = _config->getBetaChains()[i];
                makeBetaDecay(item->iGasIndex1, item->iGasIndex2, item->dLambda1);
                makeBetaDecay(item->iGasIndex2, item->iGasIndex3, item->dLambda2);
            }
        }

        checkCells();

        auto now = std::chrono::steady_clock::now();
        auto wholeTime = std::chrono::duration_cast<std::chrono::seconds>(now - startTimestamp).count();
        auto iterationTime = std::chrono::duration_cast<std::chrono::milliseconds>(now - timestamp).count();

        std::cout << "run() : " << it << "/" << Config::getInstance()->getMaxIteration() << std::endl;
        std::cout << "Iteration time: " << iterationTime << " ms" << std::endl;
        std::cout << "Inner time: " << it * _config->getNormalizer()->restore(static_cast<const double&>(_config->getTimestep()), Normalizer::Type::TIME) << " s" << std::endl;
        std::cout << "Real time: " << wholeTime << " s" << std::endl;
        std::cout << "Remaining time: ";
        if (it == 0) {
            std::cout << "Unknown";
        } else {
            std::cout << wholeTime * (_config->getMaxIteration() - it) / it / 60 << " m";
        }
        std::cout << std::endl;
        std::cout << std::endl;
    }

    std::cout << "Done" << std::endl;
}

void Solver::initCellType(sep::Axis axis) {
    const std::vector<Cell*>& cells = _grid->getValues();

    // make type
    for (auto item : cells) {
        if (item != nullptr) {
            item->computeType(axis);
        }
    }
}

void Solver::makeStep(sep::Axis axis) {
    const std::vector<Cell*>& cells = _grid->getValues();

    // Make half
    for (auto item : cells) {
        if (item != nullptr) {
            item->computeHalf(axis);
        }
    }

    // Make value
    for (auto item : cells) {
        if (item != nullptr) {
            item->computeValue(axis);
        }
    }
}

void Solver::makeIntegral(unsigned int gi0, unsigned int gi1, double timestep) {
    const GasVector& vGases = _config->getGases();
    ci::Particle cParticle{};
    cParticle.d = 1.;

    ci::gen(timestep, 50000,
            _impulse->getResolution() / 2, _impulse->getResolution() / 2,
            _impulse->getXYZ2I(), _impulse->getXYZ2I(),
            _impulse->getMaxImpulse() / (_impulse->getResolution() / 2),
            vGases[gi0]->getMass(), vGases[gi1]->getMass(),
            cParticle, cParticle);

    const std::vector<Cell*>& cells = _grid->getValues();
    for (auto item : cells) {
        if (item != nullptr) {
            item->computeIntegral(gi0, gi1);
        }
    }
}

void Solver::makeBetaDecay(unsigned int gi0, unsigned int gi1, double lambda) {
    const std::vector<Cell*>& cells = _grid->getValues();
    for (auto item : cells) {
        if (item != nullptr) {
            item->computeBetaDecay(gi0, gi1, lambda);
        }
    }
}

void Solver::checkCells() {
    const std::vector<Cell*>& cells = _grid->getValues();
    for (auto item : cells) {
        if (item != nullptr) {
            item->checkInnerValuesRange();
        }
    }
}
