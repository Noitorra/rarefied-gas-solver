#include "BorderCell.h"
#include "CellConnection.h"

#include <stdexcept>

BorderCell::BorderCell(int id) : BaseCell(Type::BORDER, id) {
    const auto& gases = Config::getInstance()->getGases();
    _borderTypes.resize(gases.size(), BorderType::UNDEFINED);
}

void BorderCell::init() {
    auto config = Config::getInstance();
    const auto& gases = config->getGases();
    const auto& impulses = config->getImpulseSphere()->getImpulses();

    // Allocating space for values and border types
    _values.resize(gases.size());

    for (unsigned int gi = 0; gi < gases.size(); gi++) {
        _values[gi].resize(impulses.size(), 0.0);
    }
}

void BorderCell::computeTransfer() {
    if (_connections.size() != 1) {
        throw std::runtime_error("wrong border connection");
    }

    auto config = Config::getInstance();
    const auto& gases = config->getGases();

    for (unsigned int gi = 0; gi < gases.size(); gi++) {
        switch (_borderTypes[gi]) {
            case BorderType::UNDEFINED:
                throw std::runtime_error("undefined border type");

            case BorderType::DIFFUSE:
                computeTransferDiffuse(gi);
                break;

            case BorderType::MIRROR:
                computeTransferMirror(gi);
                break;

            case BorderType::PRESSURE:
                computeTransferPressure(gi);
                break;

            case BorderType::FLOW:
                computeTransferFlow(gi);
                break;
        }
    }
}

void BorderCell::computeIntegral(int gi0, int gi1) {
    // nothing
}

void BorderCell::computeBetaDecay(int gi0, int gi1, double lambda) {
    // nothing
}

void BorderCell::setBorderType(int gi, BorderCell::BorderType borderType) {
    _borderTypes[gi] = borderType;
}

CellParameters& BorderCell::getBoundaryParams() {
    return _boundaryParams;
}

void BorderCell::computeTransferDiffuse(unsigned int gi) {
    const auto& connection = _connections[0];

    auto config = Config::getInstance();
    const auto& gases = config->getGases();
    const auto& impulses = config->getImpulseSphere()->getImpulses();

    double cUp = 0.0, cDown = 0.0;
    for (unsigned int ii = 0; ii < impulses.size(); ii++) {
        double projection = impulses[ii].scalar(connection->getNormal12());
        if (projection < 0.0) {
            cUp += -projection * connection->getSecond()->getValues()[gi][ii];
        } else {
            cDown += projection * fast_exp(gases[gi].getMass(), _boundaryParams.getTemp(gi), impulses[ii]);
        }
    }

    double h = cUp / cDown;
    for (unsigned int ii = 0; ii < impulses.size(); ii++) {
        double projection = impulses[ii].scalar(connection->getNormal12());
        if (projection >= 0.0) {
            _values[gi][ii] = h * fast_exp(gases[gi].getMass(), _boundaryParams.getTemp(gi), impulses[ii]);
        }
    }
}

void BorderCell::computeTransferMirror(unsigned int gi) {
    const auto& connection = _connections[0];

    auto config = Config::getInstance();
    const auto& gases = config->getGases();
    const auto& impulseSphere = config->getImpulseSphere();
    const auto& impulses = impulseSphere->getImpulses();

    for (unsigned int ii = 0; ii < impulses.size(); ii++) {
        double projection = impulses[ii].scalar(connection->getNormal12());
        if (projection >= 0.0) {
            auto rii = impulseSphere->reverseIndex(ii, connection->getNormal12());
            if (rii >= 0) {
                _values[gi][ii] = connection->getSecond()->getValues()[gi][rii];
            } else {
                _values[gi][ii] = 0.0;
            }
        }
    }
}

void BorderCell::computeTransferPressure(unsigned int gi) {
    const auto& connection = _connections[0];

    auto config = Config::getInstance();
    const auto& gases = config->getGases();
    const auto& impulseSphere = config->getImpulseSphere();
    const auto& impulses = impulseSphere->getImpulses();

    double cUp0 = _boundaryParams.getPressure(gi);
    if (cUp0 > 0) {
        cUp0 /= _boundaryParams.getTemp(gi) * impulseSphere->getDeltaImpulseQube();
    }
    double cUp = cUp0, cDown = 0.0;
    for (unsigned int ii = 0; ii < impulses.size(); ii++) {
        double projection = impulses[ii].scalar(connection->getNormal12());
        if (projection < 0.0) {
            cUp -= connection->getSecond()->getValues()[gi][ii];
        } else {
            cDown += fast_exp(gases[gi].getMass(), _boundaryParams.getTemp(gi), impulses[ii]);
        }
    }

    double h = cUp / cDown;
    if (h > 0) {
        for (unsigned int ii = 0; ii < impulses.size(); ii++) {
            double projection = impulses[ii].scalar(connection->getNormal12());
            if (projection >= 0.0) {
                _values[gi][ii] = h * fast_exp(gases[gi].getMass(), _boundaryParams.getTemp(gi), impulses[ii]);
            }
        }
    } else {
        for (unsigned int ii = 0; ii < impulses.size(); ii++) {
            double projection = impulses[ii].scalar(connection->getNormal12());
            if (projection >= 0.0) {
                _values[gi][ii] = 0.0;
            }
        }
    }
}

void BorderCell::computeTransferFlow(unsigned int gi) {
    const auto& connection = _connections[0];

    auto config = Config::getInstance();
    const auto& gases = config->getGases();
    const auto& impulseSphere = config->getImpulseSphere();
    const auto& impulses = impulseSphere->getImpulses();

    double cUp0 = _boundaryParams.getFlow(gi).scalar(connection->getNormal12());
    if (cUp0 > 0) {
        cUp0 /= impulseSphere->getDeltaImpulseQube();
    }
    double cUp = cUp0, cDown = 0.0;
    for (unsigned int ii = 0; ii < impulses.size(); ii++) {
        double projection = impulses[ii].scalar(connection->getNormal12());
        if (projection < 0.0) {
            cUp += -projection * connection->getSecond()->getValues()[gi][ii];
        } else {
            cDown += projection * fast_exp(gases[gi].getMass(), _boundaryParams.getTemp(gi), impulses[ii]);
        }
    }

    double h = cUp / cDown;
    if (h > 0) {
        for (unsigned int ii = 0; ii < impulses.size(); ii++) {
            double projection = impulses[ii].scalar(connection->getNormal12());
            if (projection >= 0.0) {
                _values[gi][ii] = h * fast_exp(gases[gi].getMass(), _boundaryParams.getTemp(gi), impulses[ii]);
            }
        }
    } else {
        for (unsigned int ii = 0; ii < impulses.size(); ii++) {
            double projection = impulses[ii].scalar(connection->getNormal12());
            if (projection >= 0.0) {
                _values[gi][ii] = 0.0;
            }
        }
    }
}
