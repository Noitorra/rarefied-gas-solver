#include "BorderCell.h"
#include "CellConnection.h"

BorderCell::BorderCell(BorderCell::BorderType borderType) : BaseCell(Type::BORDER, 0) {
    _borderType = borderType;
}

void BorderCell::init() {
    auto config = Config::getInstance();
    const auto& gases = config->getGases();
    const auto& impulses = config->getImpulse()->getVector();

    // Allocating space for values and new values
    _values.resize(gases.size());

    for (unsigned int gi = 0; gi < gases.size(); gi++) {
        _values[gi].resize(impulses.size(), 0.0);
    }
}

void BorderCell::computeTransfer() {
    if (_connections.size() != 1) {
        throw std::runtime_error("wrong border connection");
    }
    const auto& connection = _connections[0];

    auto config = Config::getInstance();
    const auto& gases = config->getGases();
    const auto& impulses = config->getImpulse()->getVector();

    double cUp = 0.0, cDown = 0.0;
    for (unsigned int gi = 0; gi < gases.size(); gi++) {
        for (unsigned int ii = 0; ii < impulses.size(); ii++) {
            double projection = impulses[ii].scalar(connection->getNormal12());
            if (projection < 0.0) {
                cUp += -projection * connection->getSecond()->getValues()[gi][ii];
            } else {
                cDown += projection * fast_exp(gases[gi].getMass(), _boundaryParams.getTemp(gi), impulses[ii]);
            }
        }
    }

    double h = cUp / cDown;
    for (unsigned int gi = 0; gi < gases.size(); gi++) {
        for (unsigned int ii = 0; ii < impulses.size(); ii++) {
            double projection = impulses[ii].scalar(connection->getNormal12());
            if (projection >= 0.0) {
                _values[gi][ii] = h * fast_exp(gases[gi].getMass(), _boundaryParams.getTemp(gi), impulses[ii]);
            }
        }
    }
}

void BorderCell::computeIntegral(unsigned int gi0, unsigned int gi1) {
    // nothing
}

void BorderCell::computeBetaDecay(unsigned int gi0, unsigned int gi1, double lambda) {
    // nothing
}

CellParameters& BorderCell::getBoundaryParams() {
    return _boundaryParams;
}
