#include "BorderCell.h"
#include "CellConnection.h"
#include "NormalCell.h"

#include <stdexcept>

void BorderCell::init() {
    auto config = Config::getInstance();
    const auto& gases = config->getGases();
    const auto& impulses = config->getImpulseSphere()->getImpulses();

    // Allocating space for values and border types
    _values.resize(gases.size());
    for (unsigned int gi = 0; gi < gases.size(); gi++) {
        _values[gi].resize(impulses.size(), 0.0);
    }

    // cache exponent
    _cacheExp.resize(gases.size());
    for (unsigned int gi = 0; gi < gases.size(); gi++) {
        _cacheExp[gi].resize(impulses.size(), 0.0);
        for (unsigned int ii = 0; ii < impulses.size(); ii++) {
            _cacheExp[gi][ii] = std::exp(-impulses[ii].moduleSquare() / gases[gi].getMass() / 2 / _boundaryParams.getTemp(gi));
        }
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
                computeTransferPressure(gi, _boundaryParams.getPressure(gi));
                break;

            case BorderType::FLOW:
                computeTransferFlow(gi, _boundaryParams.getFlow(gi).scalar(_connections[0]->getNormal12()));
                break;

            case BorderType::FLOW_CONNECT:
                _gridBuffer->addFlow(_group, gi, computeTransferFlowConnect(gi, _gridBuffer->getAverageFlow(_connectGroups[gi], gi)));
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

void BorderCell::computeImplicitTransfer(int ii) {
    // nothing, all must be calculated before
}

void BorderCell::computeTransferDiffuse(unsigned int gi) {
    auto config = Config::getInstance();
    const auto& gases = config->getGases();
    const auto& impulses = config->getImpulseSphere()->getImpulses();

    double cUp = 0.0, cDown = 0.0;
    for (unsigned int ii = 0; ii < impulses.size(); ii++) {
        double projection = impulses[ii].scalar(_connections[0]->getNormal12());
        if (projection < 0.0) {
            cUp += -projection * _connections[0]->getSecond()->getValues()[gi][ii];
        } else {
            cDown += projection * _cacheExp[gi][ii];
        }
    }

    double h = cUp / cDown;
    for (unsigned int ii = 0; ii < impulses.size(); ii++) {
        double projection = impulses[ii].scalar(_connections[0]->getNormal12());
        if (projection >= 0.0) {
            _values[gi][ii] = h * _cacheExp[gi][ii];
        }
    }
}

void BorderCell::computeTransferMirror(unsigned int gi) {
    auto config = Config::getInstance();
    const auto& gases = config->getGases();
    const auto& impulseSphere = config->getImpulseSphere();
    const auto& impulses = impulseSphere->getImpulses();

    for (unsigned int ii = 0; ii < impulses.size(); ii++) {
        double projection = impulses[ii].scalar(_connections[0]->getNormal12());
        if (projection >= 0.0) {
            auto rii = impulseSphere->reverseIndex(ii, _connections[0]->getNormal12());
            if (rii >= 0) {
                _values[gi][ii] = _connections[0]->getSecond()->getValues()[gi][rii];
            } else {
                _values[gi][ii] = 0.0;
            }
        }
    }
}

void BorderCell::computeTransferPressure(unsigned int gi, double borderPressure) {
    auto config = Config::getInstance();
    const auto& gases = config->getGases();
    const auto& impulseSphere = config->getImpulseSphere();
    const auto& impulses = impulseSphere->getImpulses();

    double coeff = 0.0;
    for (unsigned int ii = 0; ii < impulses.size(); ii++) {
        coeff += _cacheExp[gi][ii];
    }
    coeff = 1.0 / coeff;
    coeff *= borderPressure / _boundaryParams.getTemp(gi) / config->getImpulseSphere()->getDeltaImpulseQube();
    for (unsigned int ii = 0; ii < impulses.size(); ii++) {
        double projection = impulses[ii].scalar(_connections[0]->getNormal12());
        if (projection >= 0.0) {
            _values[gi][ii] = coeff * _cacheExp[gi][ii];
        }
    }

//    if (borderPressure > 0) {
//        borderPressure /= _boundaryParams.getTemp(gi) * impulseSphere->getDeltaImpulseQube();
//    }
//    double cUp = borderPressure, cDown = 0.0;
//    for (unsigned int ii = 0; ii < impulses.size(); ii++) {
//        double projection = impulses[ii].scalar(_connections[0]->getNormal12());
//        if (projection< 0.0) {
//            cUp -= _connections[0]->getSecond()->getValues()[gi][ii];
//        } else {
//            cDown += _cacheExp[gi][ii];
//        }
//    }
//
//    double h = cUp / cDown;
//    if (h > 0) {
//        for (unsigned int ii = 0; ii < impulses.size(); ii++) {
//            double projection = impulses[ii].scalar(_connections[0]->getNormal12());
//            if (projection >= 0.0) {
//                _values[gi][ii] = h * _cacheExp[gi][ii];
//            }
//        }
//    } else {
//        for (unsigned int ii = 0; ii < impulses.size(); ii++) {
//            double projection = impulses[ii].scalar(_connections[0]->getNormal12());
//            if (projection >= 0.0) {
//                _values[gi][ii] = 0.0;
//            }
//        }
//    }
}

void BorderCell::computeTransferFlow(unsigned int gi, double borderFlow) {
    auto config = Config::getInstance();
    const auto& gases = config->getGases();
    const auto& impulseSphere = config->getImpulseSphere();
    const auto& impulses = impulseSphere->getImpulses();

    double cUp = 0.0, cDown = 0.0;
    for (unsigned int ii = 0; ii < impulses.size(); ii++) {
        double projection = impulses[ii].scalar(_connections[0]->getNormal12());
        if (projection < 0.0) {
            cUp += -projection * _connections[0]->getSecond()->getValues()[gi][ii];
        } else {
            cDown += projection * _cacheExp[gi][ii];
        }
    }

    // add flow from border to output
    double h = (cUp + borderFlow / impulseSphere->getDeltaImpulseQube()) / cDown;
    if (h > 0) {
        for (unsigned int ii = 0; ii < impulses.size(); ii++) {
            double projection = impulses[ii].scalar(_connections[0]->getNormal12());
            if (projection >= 0.0) {
                _values[gi][ii] = h * _cacheExp[gi][ii];
            }
        }
    } else {
        for (unsigned int ii = 0; ii < impulses.size(); ii++) {
            double projection = impulses[ii].scalar(_connections[0]->getNormal12());
            if (projection >= 0.0) {
                _values[gi][ii] = 0.0;
            }
        }
    }
}

double BorderCell::computeTransferFlowConnect(unsigned int gi, double borderFlow) {
    auto config = Config::getInstance();
    const auto& gases = config->getGases();
    const auto& impulseSphere = config->getImpulseSphere();
    const auto& impulses = impulseSphere->getImpulses();

    double cUp = 0.0, cDown = 0.0;
    for (unsigned int ii = 0; ii < impulses.size(); ii++) {
        double projection = impulses[ii].scalar(_connections[0]->getNormal12());
        if (projection < 0.0) {
            cUp += -projection * _connections[0]->getSecond()->getValues()[gi][ii];
        } else {
            cDown += projection * _cacheExp[gi][ii];
        }
    }

    // add flow from border to output
    double h = (borderFlow / impulseSphere->getDeltaImpulseQube()) / cDown;
    if (h > 0) {
        for (unsigned int ii = 0; ii < impulses.size(); ii++) {
            double projection = impulses[ii].scalar(_connections[0]->getNormal12());
            if (projection >= 0.0) {
                _values[gi][ii] = h * _cacheExp[gi][ii];
            }
        }
    } else {
        for (unsigned int ii = 0; ii < impulses.size(); ii++) {
            double projection = impulses[ii].scalar(_connections[0]->getNormal12());
            if (projection >= 0.0) {
                _values[gi][ii] = 0.0;
            }
        }
    }

    return cUp * impulseSphere->getDeltaImpulseQube();
}
