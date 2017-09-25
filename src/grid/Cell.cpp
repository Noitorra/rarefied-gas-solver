#include <utilities/Utils.h>
#include "Cell.h"

#include "parameters/Gas.h"
#include "parameters/Impulse.h"

#include "integral/ci.hpp"
#include "CellData.h"

Cell::Cell(CellData* data) : _data(data), _config(Config::getInstance()) {
    _computationType.resize(3, ComputationType::UNDEFINED);
    _next.resize(3, nullptr);
    _prev.resize(3, nullptr);
}

void Cell::init() {
    const std::vector<Gas>& vGases = _config->getGases();
    unsigned int iGasesCount = _config->getGasesCount();
    Impulse* pImpulse = _config->getImpulse();
    const std::vector<Vector3d>& vImpulses = pImpulse->getVector();

    // Allocating space for values and half's
    _halfValues.resize(iGasesCount);
    _values.resize(iGasesCount);

    for (unsigned int gi = 0; gi < iGasesCount; gi++) {
        _halfValues[gi].resize(vImpulses.size(), 0.0);
        _values[gi].resize(vImpulses.size(), 0.0);

        double dDensity = 0.0;
        for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
            dDensity += compute_exp(vGases[gi].getMass(), _data->params().getTemp(gi), vImpulses[ii]);
        }

        dDensity *= pImpulse->getDeltaImpulseQube();
        dDensity = _data->params().getPressure(gi) / _data->params().getTemp(gi) / dDensity;

        for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
            _values[gi][ii] = dDensity * compute_exp(
                    vGases[gi].getMass(),
                    _data->params().getTemp(gi),
                    vImpulses[ii]
            );
        }
    }
}

void Cell::link(unsigned int axis, Cell* prevCell, Cell* nextCell) {
    _prev[axis] = prevCell;
    _next[axis] = nextCell;
}

CellData* Cell::getData() {
    return _data;
}

void Cell::computeType(unsigned int axis) {
    compute_type(axis);
}

void Cell::computeHalf(unsigned int axis) {
    switch (_computationType[axis]) {
        case ComputationType::LEFT:
            compute_half_left(axis);
            break;
        case ComputationType::NORMAL:
            compute_half_normal(axis);
            break;
        case ComputationType::PRE_RIGHT:
            compute_half_preright(axis);
            break;
        case ComputationType::RIGHT:
            compute_half_right(axis);
            break;
        default:
            break;
    }
}

void Cell::computeValue(unsigned int axis) {
    switch (_computationType[axis]) {
        case ComputationType::NORMAL:
            compute_value_normal(axis);
            break;
        case ComputationType::PRE_RIGHT:
            compute_value_normal(axis);
            break;
        default:
            break;
    }
}

void Cell::computeIntegral(unsigned int gi0, unsigned int gi1) {
    ci::iter(_values[gi0], _values[gi1]);
}

void Cell::computeBetaDecay(unsigned int gi0, unsigned int gi1, double lambda) {
    const std::vector<Vector3d>& vImpulse = _config->getImpulse()->getVector();

    double dTempValue = 0.0;
    for (unsigned int ii = 0; ii < vImpulse.size(); ii++) {
        dTempValue = _values[gi0][ii] * lambda * _config->getTimestep(); // TODO: check lambda * Config::m_dTimestep < 1 !!!
        _values[gi0][ii] -= dTempValue;
        _values[gi1][ii] += dTempValue;
    }
}

CellParameters& Cell::getResultParams() {
    _resultParams.reset();

    if (_data->isNormal() == true) {
        for (unsigned int gi = 0; gi < _config->getGasesCount(); gi++) {
            double density = compute_concentration(gi);
            Vector3d flow = compute_stream(gi);
            double temp = 0.0;
            double pressure = 0.0;
            Vector3d heatFlow = Vector3d();
            if (density > 0.0) {
                temp = compute_temperature(gi, density, flow);
                pressure = compute_pressure(gi, density, temp);
                heatFlow = compute_heatstream(gi);
            }

            _resultParams.set(gi, pressure, density, temp, flow, heatFlow);
        }
    }

    return _resultParams;
}

bool Cell::checkValuesRange() {
    int iGasesCount = _config->getGasesCount();
    const std::vector<Vector3d>& vImpulses = _config->getImpulse()->getVector();

    bool result = true;

    for (unsigned int gi = 0; gi < iGasesCount; gi++) {
        for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
            if (_values[gi][ii] < 0) {

                // wrong values
                result = false;
                std::cout << "Cell::checkValuesRange(): Error [type][gi][ii]"
                          << "[" << Utils::asNumber(_computationType[0])
                          << ":" << Utils::asNumber(_computationType[1])
                          << ":" << Utils::asNumber(_computationType[2]) << "]"
                          << "[" << gi << "]"
                          << "[" << ii << "]"
                          << " value = " << _values[gi][ii] << std::endl;
            }
            if (_halfValues[gi][ii] < 0) {

                // wrong values
                result = false;
                std::cout << "Cell::checkValuesRange(): Error [type][gi][ii]"
                          << "[" << Utils::asNumber(_computationType[0])
                          << ":" << Utils::asNumber(_computationType[1])
                          << ":" << Utils::asNumber(_computationType[2]) << "]"
                          << "[" << gi << "]"
                          << "[" << ii << "]"
                          << " half = " << _halfValues[gi][ii] << std::endl;
            }
        }
    }

    return result;
}

std::vector<DoubleVector>& Cell::getValues() {
    return _values;
}

std::vector<DoubleVector>& Cell::getHalfValues() {
    return _halfValues;
}

// private ------------------------------------------------------------------------------------------------------------------------------------------

void Cell::compute_type(unsigned int axis) {
    if (_prev[axis] == nullptr) {
        if (_next[axis] != nullptr) {
            _computationType[axis] = ComputationType::LEFT;
        }
    } else {
        if (_next[axis] == nullptr) {
            _computationType[axis] = ComputationType::RIGHT;
        } else {
            if (_next[axis]->_next[axis] == nullptr) {
                _computationType[axis] = ComputationType::PRE_RIGHT;
            } else {
                _computationType[axis] = ComputationType::NORMAL;
            }
        }
    }
}

void Cell::compute_half_left(unsigned int axis) {
    int iGasesCount = _config->getGasesCount();

    for (unsigned int gi = 0; gi < iGasesCount; gi++) {
        switch (_data->getBoundaryType(gi)) {
            case CellData::BoundaryType::DIFFUSE:
                compute_half_diffuse_left(axis, gi);
                break;
            case CellData::BoundaryType::PRESSURE:
                compute_half_gase_left(axis, gi);
                break;
            case CellData::BoundaryType::MIRROR:
                compute_half_mirror_left(axis, gi);
                break;
            case CellData::BoundaryType::FLOW:
                compute_half_flow_left(axis, gi);
                break;
        }
    }
}

void Cell::compute_half_normal(unsigned int axis) {
    const std::vector<Gas>& vGases = _config->getGases();
    int iGasesCount = _config->getGasesCount();
    const std::vector<Vector3d>& vImpulses = _config->getImpulse()->getVector();

    for (unsigned int gi = 0; gi < iGasesCount; gi++) {
        for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
            double y = _config->getTimestep() / vGases[gi].getMass() * std::abs(vImpulses[ii].get(axis) / _data->getStep().get(axis));

            if (vImpulses[ii].get(axis) > 0) {
                _halfValues[gi][ii] = _values[gi][ii] + (1 - y) / 2 * limiter(
                        _prev[axis]->_values[gi][ii],
                        _values[gi][ii],
                        _next[axis]->_values[gi][ii]
                );
            } else {
                _halfValues[gi][ii] = _next[axis]->_values[gi][ii] - (1 - y) / 2 * limiter(
                        _values[gi][ii],
                        _next[axis]->_values[gi][ii],
                        _next[axis]->_next[axis]->_values[gi][ii]
                );
            }
        }
    }
}

void Cell::compute_half_preright(unsigned int axis) {}

void Cell::compute_half_right(unsigned int axis) {
    int iGasesCount = _config->getGasesCount();

    for (unsigned int gi = 0; gi < iGasesCount; gi++) {
        switch (_data->getBoundaryType(gi)) {
            case CellData::BoundaryType::DIFFUSE:
                compute_half_diffuse_right(axis, gi);
                break;
            case CellData::BoundaryType::PRESSURE:
                compute_half_gase_right(axis, gi);
                break;
            case CellData::BoundaryType::MIRROR:
                compute_half_mirror_right(axis, gi);
                break;
            case CellData::BoundaryType::FLOW:
                compute_half_flow_right(axis, gi);
                break;
        }
    }
}

void Cell::compute_value_normal(unsigned int axis) {
    const std::vector<Gas>& vGases = _config->getGases();
    int iGasesCount = _config->getGasesCount();
    const std::vector<Vector3d>& vImpulses = _config->getImpulse()->getVector();

    for (unsigned int gi = 0; gi < iGasesCount; gi++) {
        for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
            double y = _config->getTimestep() / vGases[gi].getMass() * vImpulses[ii].get(axis) / _data->getStep().get(axis);
            _values[gi][ii] = _values[gi][ii] - y * (_halfValues[gi][ii] - _prev[axis]->_halfValues[gi][ii]);
        }
    }
}

void Cell::compute_half_diffuse_left(unsigned int axis, unsigned int gi) {
    const std::vector<Gas>& vGases = _config->getGases();
    const std::vector<Vector3d>& vImpulses = _config->getImpulse()->getVector();

    double C1_up = 0.0;
    double C1_down = 0.0;
    double C2_up = 0.0;
    for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
        if (vImpulses[ii].get(axis) < 0) {
            double y = _config->getTimestep() / vGases[gi].getMass() * std::abs(vImpulses[ii].get(axis) / _data->getStep().get(axis));

            _values[gi][ii] = 2 * _next[axis]->_values[gi][ii] - _next[axis]->_next[axis]->_values[gi][ii];
            if (_values[gi][ii] < 0)
                _values[gi][ii] = 0;

            _halfValues[gi][ii] = _next[axis]->_values[gi][ii] - (1 - y) / 2 * limiter(
                    _values[gi][ii],
                    _next[axis]->_values[gi][ii],
                    _next[axis]->_next[axis]->_values[gi][ii]);

            C1_up += std::abs(vImpulses[ii].get(axis) * _halfValues[gi][ii]);
            C2_up += std::abs(vImpulses[ii].get(axis) * (_values[gi][ii] + _next[axis]->_values[gi][ii]) / 2);
        } else {
            C1_down += std::abs(vImpulses[ii].get(axis) * compute_exp(
                    vGases[gi].getMass(),
                    _data->boundaryParams().getTemp(gi),
                    vImpulses[ii]
            ));
        }
    }

    for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
        if (vImpulses[ii].get(axis) > 0) {
            _halfValues[gi][ii] = C1_up / C1_down * compute_exp(
                    vGases[gi].getMass(),
                    _data->boundaryParams().getTemp(gi),
                    vImpulses[ii]
            );
            _values[gi][ii] = 2 * C2_up / C1_down * compute_exp(
                    vGases[gi].getMass(),
                    _data->boundaryParams().getTemp(gi),
                    vImpulses[ii]
            ) - _next[axis]->_values[gi][ii];
            if (_values[gi][ii] < 0.0)
                _values[gi][ii] = 0.0;
        }
    }
}

void Cell::compute_half_diffuse_right(unsigned int axis, unsigned int gi) {
    const std::vector<Gas>& vGases = _config->getGases();
    const std::vector<Vector3d>& vImpulses = _config->getImpulse()->getVector();

    double C1_up = 0.0;
    double C1_down = 0.0;
    double C2_up = 0.0;
    for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
        if (vImpulses[ii].get(axis) > 0) {
            double y = _config->getTimestep() / vGases[gi].getMass() * std::abs(vImpulses[ii].get(axis) / _data->getStep().get(axis));

            _values[gi][ii] = 2 * _prev[axis]->_values[gi][ii] - _prev[axis]->_prev[axis]->_values[gi][ii];
            if (_values[gi][ii] < 0)
                _values[gi][ii] = 0;

            _prev[axis]->_halfValues[gi][ii] = _prev[axis]->_values[gi][ii] + (1 - y) / 2 * limiter(
                    _prev[axis]->_prev[axis]->_values[gi][ii],
                    _prev[axis]->_values[gi][ii],
                    _values[gi][ii]);

            C1_up += std::abs(vImpulses[ii].get(axis) * _prev[axis]->_halfValues[gi][ii]);
            C2_up += std::abs(vImpulses[ii].get(axis) * (_values[gi][ii] + _prev[axis]->_values[gi][ii]) / 2);
        } else {
            C1_down += std::abs(vImpulses[ii].get(axis) * compute_exp(
                    vGases[gi].getMass(),
                    _data->boundaryParams().getTemp(gi),
                    vImpulses[ii]
            ));
        }
    }

    for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
        if (vImpulses[ii].get(axis) < 0) {
            _prev[axis]->_halfValues[gi][ii] = C1_up / C1_down * compute_exp(
                    vGases[gi].getMass(),
                    _data->boundaryParams().getTemp(gi),
                    vImpulses[ii]
            );
            _values[gi][ii] = 2 * C2_up / C1_down * compute_exp(
                    vGases[gi].getMass(),
                    _data->boundaryParams().getTemp(gi),
                    vImpulses[ii]
            ) - _prev[axis]->_values[gi][ii];
            if (_values[gi][ii] < 0.0) _values[gi][ii] = 0.0;
        }
    }
}

void Cell::compute_half_gase_left(unsigned int axis, unsigned int gi) {
    const std::vector<Gas>& vGases = _config->getGases();
    Impulse* pImpulse = _config->getImpulse();
    const std::vector<Vector3d>& vImpulses = pImpulse->getVector();

    double C1_up = 0.0;
    double C1_down = 0.0;
    double C2_up = 0.0;

    Vector3d v3Speed = Vector3d();
    if (_data->boundaryParams().getPressure(gi) > 0.0) {
        v3Speed = _data->boundaryParams().getFlow(gi) / (_data->boundaryParams().getPressure(gi) / _data->boundaryParams().getTemp(gi));
    }

    for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
        if (vImpulses[ii].get(axis) < 0) {
            double y = _config->getTimestep() / vGases[gi].getMass() * std::abs(vImpulses[ii].get(axis) / _data->getStep().get(axis));

            _values[gi][ii] = 2 * _next[axis]->_values[gi][ii] - _next[axis]->_next[axis]->_values[gi][ii];
            if (_values[gi][ii] < 0)
                _values[gi][ii] = 0;

            _halfValues[gi][ii] = _next[axis]->_values[gi][ii] - (1 - y) / 2 * limiter(
                    _values[gi][ii],
                    _next[axis]->_values[gi][ii],
                    _next[axis]->_next[axis]->_values[gi][ii]);

            C1_up += _halfValues[gi][ii];
            C2_up += (_values[gi][ii] + _next[axis]->_values[gi][ii]) / 2;
        } else {
            C1_down += compute_exp(
                    vGases[gi].getMass(),
                    _data->boundaryParams().getTemp(gi),
                    vImpulses[ii] - v3Speed * vGases[gi].getMass());
        }
    }

    // Vacuum
    if (_data->boundaryParams().getPressure(gi) == 0.0) {
        C1_up = 0.0;
        C2_up = 0.0;
    } else {
        C1_up = _data->boundaryParams().getPressure(gi) / _data->boundaryParams().getTemp(gi) / pImpulse->getDeltaImpulseQube() - C1_up;
        C2_up = _data->boundaryParams().getPressure(gi) / _data->boundaryParams().getTemp(gi) / pImpulse->getDeltaImpulseQube() - C2_up;

        if (C1_up < 0.0)
            C1_up = 0.0;
        if (C2_up < 0.0)
            C2_up = 0.0;
    }

    for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
        if (vImpulses[ii].get(axis) > 0) {
            _halfValues[gi][ii] = C1_up / C1_down * compute_exp(
                    vGases[gi].getMass(),
                    _data->boundaryParams().getTemp(gi),
                    vImpulses[ii] - v3Speed * vGases[gi].getMass());

            _values[gi][ii] = 2 * C2_up / C1_down * compute_exp(
                    vGases[gi].getMass(),
                    _data->boundaryParams().getTemp(gi),
                    vImpulses[ii] - v3Speed * vGases[gi].getMass()) - _next[axis]->_values[gi][ii];
            if (_values[gi][ii] < 0.0)
                _values[gi][ii] = 0.0;
        }
    }
}

void Cell::compute_half_gase_right(unsigned int axis, unsigned int gi) {
    const std::vector<Gas>& vGases = _config->getGases();
    Impulse* pImpulse = _config->getImpulse();
    const std::vector<Vector3d>& vImpulses = pImpulse->getVector();

    double C1_up = 0.0;
    double C1_down = 0.0;
    double C2_up = 0.0;

    Vector3d v3Speed = Vector3d();
    if (_data->boundaryParams().getPressure(gi) > 0.0) {
        v3Speed = _data->boundaryParams().getFlow(gi) / (_data->boundaryParams().getPressure(gi) / _data->boundaryParams().getTemp(gi));
    }

    for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
        if (vImpulses[ii].get(axis) > 0) {
            double y = _config->getTimestep() / vGases[gi].getMass() * std::abs(vImpulses[ii].get(axis) / _data->getStep().get(axis));

            _values[gi][ii] = 2 * _prev[axis]->_values[gi][ii] - _prev[axis]->_prev[axis]->_values[gi][ii];
            if (_values[gi][ii] < 0)
                _values[gi][ii] = 0;

            _prev[axis]->_halfValues[gi][ii] = _prev[axis]->_values[gi][ii] + (1 - y) / 2 * limiter(
                    _prev[axis]->_prev[axis]->_values[gi][ii],
                    _prev[axis]->_values[gi][ii],
                    _values[gi][ii]);

            C1_up += _prev[axis]->_halfValues[gi][ii];
            C2_up += (_values[gi][ii] + _prev[axis]->_values[gi][ii]) / 2;
        } else {
            C1_down += compute_exp(
                    vGases[gi].getMass(),
                    _data->boundaryParams().getTemp(gi),
                    vImpulses[ii] - v3Speed * vGases[gi].getMass());
        }
    }

    // Vacuum
    if (_data->boundaryParams().getPressure(gi) == 0.0) {
        C1_up = 0.0;
        C2_up = 0.0;
    } else {
        C1_up = _data->boundaryParams().getPressure(gi) / _data->boundaryParams().getTemp(gi) / pImpulse->getDeltaImpulseQube() - C1_up;
        C2_up = _data->boundaryParams().getPressure(gi) / _data->boundaryParams().getTemp(gi) / pImpulse->getDeltaImpulseQube() - C2_up;

        if (C1_up < 0.0)
            C1_up = 0.0;
        if (C2_up < 0.0)
            C2_up = 0.0;
    }

    for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
        if (vImpulses[ii].get(axis) < 0) {
            _prev[axis]->_halfValues[gi][ii] = C1_up / C1_down * compute_exp(
                    vGases[gi].getMass(),
                    _data->boundaryParams().getTemp(gi),
                    vImpulses[ii] - v3Speed * vGases[gi].getMass());

            _values[gi][ii] = 2 * C2_up / C1_down * compute_exp(
                    vGases[gi].getMass(),
                    _data->boundaryParams().getTemp(gi),
                    vImpulses[ii] - v3Speed * vGases[gi].getMass()) - _prev[axis]->_values[gi][ii];
            if (_values[gi][ii] < 0.0)
                _values[gi][ii] = 0.0;
        }
    }
}

void Cell::compute_half_flow_left(unsigned int axis, unsigned int gi) {
    const std::vector<Gas>& vGases = _config->getGases();
    Impulse* pImpulse = _config->getImpulse();
    const std::vector<Vector3d>& vImpulses = pImpulse->getVector();

    double C1_up = 0.0;
    double C1_down = 0.0;
    double C2_up = 0.0;

    for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
        if (vImpulses[ii].get(axis) < 0) {
            double y = _config->getTimestep() / vGases[gi].getMass() * std::abs(vImpulses[ii].get(axis) / _data->getStep().get(axis));

            _values[gi][ii] = 2 * _next[axis]->_values[gi][ii] - _next[axis]->_next[axis]->_values[gi][ii];
            if (_values[gi][ii] < 0)
                _values[gi][ii] = 0;

            _halfValues[gi][ii] = _next[axis]->_values[gi][ii] - (1 - y) / 2 * limiter(
                    _values[gi][ii],
                    _next[axis]->_values[gi][ii],
                    _next[axis]->_next[axis]->_values[gi][ii]);

            C1_up += vImpulses[ii].get(axis) * _halfValues[gi][ii];
            C2_up += vImpulses[ii].get(axis) * (_values[gi][ii] + _next[axis]->_values[gi][ii]) / 2;
        } else {
            C1_down += vImpulses[ii].get(axis) * compute_exp(
                    vGases[gi].getMass(),
                    _data->boundaryParams().getTemp(gi),
                    vImpulses[ii]);
        }
    }

    C1_up = _data->boundaryParams().getFlow(gi).get(axis) / pImpulse->getDeltaImpulseQube() - C1_up;
    C2_up = _data->boundaryParams().getFlow(gi).get(axis) / pImpulse->getDeltaImpulseQube() - C2_up;

    if (C1_up / C1_down < 0.0) {
        C1_up = 0.0;
    }
    if (C2_up / C1_down < 0.0) {
        C2_up = 0.0;
    }

    for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
        if (vImpulses[ii].get(axis) > 0) {
            _halfValues[gi][ii] = C1_up / C1_down * compute_exp(
                    vGases[gi].getMass(),
                    _data->boundaryParams().getTemp(gi),
                    vImpulses[ii]);

            _values[gi][ii] = 2 * C2_up / C1_down * compute_exp(
                    vGases[gi].getMass(),
                    _data->boundaryParams().getTemp(gi),
                    vImpulses[ii]) - _next[axis]->_values[gi][ii];

            if (_values[gi][ii] < 0.0) {
                _values[gi][ii] = 0.0;
            }
        }
    }
}

void Cell::compute_half_flow_right(unsigned int axis, unsigned int gi) {
    const std::vector<Gas>& vGases = _config->getGases();
    Impulse* pImpulse = _config->getImpulse();
    const std::vector<Vector3d>& vImpulses = pImpulse->getVector();

    double C1_up = 0.0;
    double C1_down = 0.0;
    double C2_up = 0.0;

    for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
        if (vImpulses[ii].get(axis) > 0) {
            double y = _config->getTimestep() / vGases[gi].getMass() * std::abs(vImpulses[ii].get(axis) / _data->getStep().get(axis));

            _values[gi][ii] = 2 * _prev[axis]->_values[gi][ii] - _prev[axis]->_prev[axis]->_values[gi][ii];
            if (_values[gi][ii] < 0)
                _values[gi][ii] = 0;

            _prev[axis]->_halfValues[gi][ii] = _prev[axis]->_values[gi][ii] + (1 - y) / 2 * limiter(
                    _prev[axis]->_prev[axis]->_values[gi][ii],
                    _prev[axis]->_values[gi][ii],
                    _values[gi][ii]);

            C1_up += std::abs(vImpulses[ii].get(axis)) * _prev[axis]->_halfValues[gi][ii];
            C2_up += std::abs(vImpulses[ii].get(axis)) * (_values[gi][ii] + _prev[axis]->_values[gi][ii]) / 2;
        } else {
            C1_down += std::abs(vImpulses[ii].get(axis)) * compute_exp(
                    vGases[gi].getMass(),
                    _data->boundaryParams().getTemp(gi),
                    vImpulses[ii]);
        }
    }

    C1_up = _data->boundaryParams().getFlow(gi).get(axis) / pImpulse->getDeltaImpulseQube() - C1_up;
    C2_up = _data->boundaryParams().getFlow(gi).get(axis) / pImpulse->getDeltaImpulseQube() - C2_up;

    if (C1_up / C1_down < 0.0)
        C1_up = 0.0;
    if (C2_up / C1_down < 0.0)
        C2_up = 0.0;

    for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
        if (vImpulses[ii].get(axis) < 0) {
            _prev[axis]->_halfValues[gi][ii] = C1_up / C1_down * compute_exp(
                    vGases[gi].getMass(),
                    _data->boundaryParams().getTemp(gi),
                    vImpulses[ii]);

            _values[gi][ii] = 2 * C2_up / C1_down * compute_exp(
                    vGases[gi].getMass(),
                    _data->boundaryParams().getTemp(gi),
                    vImpulses[ii]) - _prev[axis]->_values[gi][ii];
            if (_values[gi][ii] < 0.0)
                _values[gi][ii] = 0.0;
        }
    }
}

void Cell::compute_half_mirror_left(unsigned int axis, unsigned int gi) {
    const std::vector<Gas>& vGases = _config->getGases();
    Impulse* pImpulse = _config->getImpulse();
    const std::vector<Vector3d>& vImpulses = pImpulse->getVector();

    for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
        double y = _config->getTimestep() / vGases[gi].getMass() * std::abs(vImpulses[ii].get(axis) / _data->getStep().get(axis));

        if (vImpulses[ii].get(axis) > 0) {
            int ri = pImpulse->reverseIndex(ii, axis);

            _halfValues[gi][ii] = _values[gi][ii] + (1 - y) / 2 * limiter(
                    _values[gi][ri], // reversed for left
                    _values[gi][ii],
                    _next[axis]->_values[gi][ii]);
        } else {
            _halfValues[gi][ii] = _next[axis]->_values[gi][ii] - (1 - y) / 2 * limiter(
                    _values[gi][ii],
                    _next[axis]->_values[gi][ii],
                    _next[axis]->_next[axis]->_values[gi][ii]);
        }
    }

    for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
        double y = _config->getTimestep() / vGases[gi].getMass() * vImpulses[ii].get(axis) / _data->getStep().get(axis);
        int ri = pImpulse->reverseIndex(ii, axis);
        _values[gi][ii] = _values[gi][ii] - y * (_halfValues[gi][ii] - _halfValues[gi][ri]);
    }
}

void Cell::compute_half_mirror_right(unsigned int axis, unsigned int gi) {
    const std::vector<Gas>& vGases = _config->getGases();
    Impulse* pImpulse = _config->getImpulse();
    const std::vector<Vector3d>& vImpulses = pImpulse->getVector();

    for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
        double y = _config->getTimestep() / vGases[gi].getMass() * std::abs(vImpulses[ii].get(axis) / _data->getStep().get(axis));

        int ri = pImpulse->reverseIndex(ii, axis);

        if (vImpulses[ii].get(axis) > 0) {

            // PREV
            _prev[axis]->_halfValues[gi][ii] = _prev[axis]->_values[gi][ii] + (1 - y) / 2 * limiter(
                    _prev[axis]->_prev[axis]->_values[gi][ii],
                    _prev[axis]->_values[gi][ii],
                    _values[gi][ii]);

            // CURRENT
            _halfValues[gi][ii] = _values[gi][ii] + (1 - y) / 2 * limiter(
                    _prev[axis]->_values[gi][ii],
                    _values[gi][ii],
                    _values[gi][ri]); // reversed for right
        } else {

            // PREV
            _prev[axis]->_halfValues[gi][ii] = _values[gi][ii] - (1 - y) / 2 * limiter(
                    _prev[axis]->_values[gi][ii],
                    _values[gi][ii],
                    _values[gi][ri]);

            // CURRENT
            _halfValues[gi][ii] = _values[gi][ri] - (1 - y) / 2 * limiter(
                    _values[gi][ii],
                    _values[gi][ri],
                    _prev[axis]->_values[gi][ii]);
        }
    }

    for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
        double y = _config->getTimestep() / vGases[gi].getMass() * vImpulses[ii].get(axis) / _data->getStep().get(axis);
        _values[gi][ii] = _values[gi][ii] - y * (_halfValues[gi][ii] - _prev[axis]->_halfValues[gi][ii]);
    }
}

inline double Cell::compute_exp(const double& mass, const double& temp, const Vector3d& impulse) {
    return std::exp(-impulse.mod2() / mass / 2 / temp);
}

inline double Cell::limiter(const double& x, const double& y, const double& z) {
    if ((z - y) * (y - x) <= 0) {
        return 0.0;
    } else {
        return Utils::sgn(z - y) * std::max(
                0.0,
                std::min(
                        2 * std::abs(y - x),
                        std::min(
                                std::abs(z - y),
                                std::min(
                                        std::abs(y - x),
                                        2 * std::abs(z - y)
                                )
                        )
                )
        );
    }
}

// Macro Data =========================================================================================================

double Cell::compute_concentration(unsigned int gi) {
    Impulse* pImpulse = _config->getImpulse();
    const std::vector<Vector3d>& vImpulses = pImpulse->getVector();

    double concentration = 0.0;
    for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
        concentration += _values[gi][ii];
    }
    concentration *= pImpulse->getDeltaImpulseQube();
    return concentration;
}

double Cell::compute_temperature(unsigned int gi, double density, const Vector3d& stream) {
    const std::vector<Gas>& vGases = _config->getGases();
    Impulse* pImpulse = _config->getImpulse();
    const std::vector<Vector3d>& vImpulses = pImpulse->getVector();

    double temperature = 0.0;
    Vector3d vAverageSpeed = stream;
    vAverageSpeed /= density;

    for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
        Vector3d vTemp;
        for (unsigned int vi = 0; vi < vAverageSpeed.getMass().size(); vi++) {
            vTemp[vi] = vImpulses[ii].get(vi) / vGases[gi].getMass() - vAverageSpeed[vi];
        }
        temperature += vGases[gi].getMass() * vTemp.mod2() * _values[gi][ii];
    }
    temperature *= pImpulse->getDeltaImpulseQube() / density / 3;

    return temperature;
}

double Cell::compute_pressure(unsigned int gi, double density, double temperature) {
    return density * temperature;
}

Vector3d Cell::compute_stream(unsigned int gi) {
    const std::vector<Gas>& vGases = _config->getGases();
    Impulse* pImpulse = _config->getImpulse();
    const std::vector<Vector3d>& vImpulses = pImpulse->getVector();

    Vector3d vStream;
    for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
        for (unsigned int vi = 0; vi < vStream.getMass().size(); vi++) {
            vStream[vi] += vImpulses[ii].get(vi) * _values[gi][ii];
        }
    }
    vStream *= pImpulse->getDeltaImpulseQube() / vGases[gi].getMass();

    return vStream;
}

Vector3d Cell::compute_heatstream(unsigned int gi) {
    return Vector3d();
}

Cell::ComputationType Cell::getComputationType(unsigned int axis) const {
    return _computationType[axis];
}
