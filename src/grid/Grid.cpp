#include "Grid.h"
#include "Cell.h"
#include "CellConnection.h"
#include "mesh/Mesh.h"
#include "parameters/Gas.h"
#include "parameters/Impulse.h"
#include "integral/ci.hpp"

Grid::Grid(Mesh* mesh) {
    // element -> cell
    for (int i = 0; i < mesh->getElementsCount(); i++) {
        Element* element = mesh->getElement(i);
        if (element->getType() == Element::Type::QUADRANGLE) {
            Cell* cell = new Cell(element->getId(), Cell::Type::NORMAL, element->getVolume());
            _cells.emplace_back(cell);
        }
    }

    // side elements -> cell connections
    for (int i = 0; i < mesh->getElementsCount(); i++) {
        Element* element = mesh->getElement(i);
        if (element->getType() == Element::Type::QUADRANGLE) {
            auto cell = getCellById(element->getId());

            for (const auto& sideElement : element->getSideElements()) {

                // find other element for this side
                const auto& elements = element->getSideElements();
                auto result = std::find_if(elements.begin(), elements.end(), [&sideElement](const std::shared_ptr<Element>& otherElement) {
                    return otherElement->contains(sideElement->getNodes());
                });
                if (result != elements.end()) {
                    Element* otherElement = result->get();
                    auto otherCell = getCellById(otherElement->getId());
                    auto connection = new CellConnection(cell, otherCell, sideElement->getVolume(), sideElement->getNormal());
                    cell->addConnection(connection);
                } else {
                    auto borderCell = new Cell(-1, Cell::Type::BORDER, 0.0);
                    _cells.emplace_back(borderCell);

                    auto connection = new CellConnection(cell, borderCell, sideElement->getVolume(), sideElement->getNormal());
                    cell->addConnection(connection);
                }
            }
        }
    }
}

const std::vector<std::shared_ptr<Cell>>& Grid::getCells() const {
    return _cells;
}

Cell* Grid::getCellById(int id) {
    auto result = std::find_if(_cells.begin(), _cells.end(), [&id](const std::shared_ptr<Cell>& cell) {
        return cell->getId() == id;
    });
    return result != _cells.end() ? result->get() : nullptr;
}

void Grid::computeTransfer() {

    // first go for border cells
    for (const auto& cell : _cells) {
        if (cell->getType() == Cell::Type::BORDER) {
            cell->computeTransfer();
        }
    }

    // then go for normal cells
    for (const auto& cell : _cells) {
        if (cell->getType() == Cell::Type::NORMAL) {
            cell->computeTransfer();
        }
    }

    // move changes from next step to current step
    for (const auto& cell : _cells) {
        if (cell->getType() == Cell::Type::NORMAL) {
            cell->swapValues();
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
            impulse->getMaxImpulse() / (impulse->getResolution() / 2),
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
