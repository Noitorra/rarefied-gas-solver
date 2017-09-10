#include <utilities/utils.h>
#include "Cell.h"

#include "parameters/gas.h"
#include "parameters/impulse.h"
#include "utilities/config.h"
#include "Parameters.h"

#include "integral/ci.hpp"
#include "CellData.h"

Cell::Cell(CellData* data) : _data(data), _config(Config::getInstance()), _lockedAxes(-1) {
    _computationType.resize(3, ComputationType::UNDEFINED);

    // Create 3 std::vector< Cell* > for dimensions
    _next.resize(3, nullptr);
    _prev.resize(3, nullptr);
}

void Cell::init() {
    const GasVector& vGases = _config->getGases();
    unsigned int iGasesNum = _config->getGasesNum();
    Impulse* pImpulse = _config->getImpulse();
    ImpulseVector& vImpulses = pImpulse->getVector();

    // Allocating space for values and half's
    _halfValue.resize(iGasesNum);
    _value.resize(iGasesNum);

    for (unsigned int gi = 0; gi < iGasesNum; gi++) {
        _halfValue[gi].resize(vImpulses.size(), 0.0);
        _value[gi].resize(vImpulses.size(), 0.0);

        double dDensity = 0.0;
        for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
            dDensity += compute_exp(vGases[gi]->getMass(), _data->getParams(gi).getTemp(), vImpulses[ii]);
        }

        dDensity *= pImpulse->getDeltaImpulseQube();
        dDensity = _data->getParams(gi).getPressure() / _data->getParams(gi).getTemp() / dDensity;

        for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
            _value[gi][ii] = dDensity * compute_exp(vGases[gi]->getMass(), _data->getParams(gi).getTemp(), vImpulses[ii]);
        }
    }
}

void Cell::link(unsigned int dim, Cell* nextCell, Cell* prevCell) {
    _next[dim] = nextCell;
    _prev[dim] = prevCell;
}

void Cell::computeType(unsigned int dim) {
    compute_type(dim);
}

void Cell::computeHalf(unsigned int dim) {
    if (dim == _lockedAxes)
        return;

    //std::cout << "ComputationType: " << _computationType[dim] << std::endl;
    switch (_computationType[dim]) {
        case ComputationType::LEFT:
            compute_half_left(dim);
            break;
        case ComputationType::NORMAL:
            compute_half_normal(dim);
            break;
        case ComputationType::PRE_RIGHT:
            compute_half_preright(dim);
            break;
        case ComputationType::RIGHT:
            compute_half_right(dim);
            break;
        default:
            break;
    }
}

void Cell::computeValue(unsigned int dim) {
    if (dim == _lockedAxes)
        return;

    switch (_computationType[dim]) {
        case ComputationType::NORMAL:
            compute_value_normal(dim);
            break;
        case ComputationType::PRE_RIGHT:
            compute_value_normal(dim);
            break;
        default:
            break;
    }
}

void Cell::computeIntegral(unsigned int gi0, unsigned int gi1) {
    ci::iter(_value[gi0], _value[gi1]);
}

void Cell::computeBetaDecay(unsigned int gi0, unsigned int gi1, double lambda) {
    ImpulseVector& impulsev = _config->getImpulse()->getVector();

    double dTempValue = 0.0;
    for (unsigned int ii = 0; ii < impulsev.size(); ii++) {
        dTempValue = _value[gi0][ii] * lambda * _config->getTimestep(); // TODO: check lambda * Config::m_dTimestep < 1 !!!
        _value[gi0][ii] -= dTempValue;
        _value[gi1][ii] += dTempValue;
    }
}

std::vector<Parameters> Cell::computeMacroData() {
    unsigned int iGasesNum = _config->getGasesNum();

    std::vector<Parameters> parameters(iGasesNum);

    if (!is_normal()) {
        for (unsigned int gi = 0; gi < iGasesNum; gi++) {
            parameters[gi].setDensity(0.0);
            parameters[gi].setFlow(Vector3d());
            parameters[gi].setTemp(0.0);
            parameters[gi].setPressure(0.0);
            parameters[gi].setHeatFlow(Vector3d());
        }
    } else {
        for (unsigned int gi = 0; gi < iGasesNum; gi++) {
            parameters[gi].setDensity(compute_concentration(gi));
            parameters[gi].setFlow(compute_stream(gi));
            parameters[gi].setTemp((parameters[gi].getDensity() == 0.0) ? 0.0 : compute_temperature(gi, parameters[gi].getDensity(), parameters[gi].getFlow()));
            parameters[gi].setPressure((parameters[gi].getDensity() == 0.0) ? 0.0 : compute_pressure(gi, parameters[gi].getDensity(), parameters[gi].getTemp()));
            parameters[gi].setHeatFlow(compute_heatstream(gi));
        }
    }

    return parameters;
}

bool Cell::is_normal() {
    bool notX = _computationType[sep::X] == ComputationType::LEFT || _computationType[sep::X] == ComputationType::RIGHT;
    bool notY = _computationType[sep::Y] == ComputationType::LEFT || _computationType[sep::Y] == ComputationType::RIGHT;
    bool notZ = _computationType[sep::Z] == ComputationType::LEFT || _computationType[sep::Z] == ComputationType::RIGHT;
    return !notX && !notY && !notZ;
}

/* Tests */
bool Cell::checkInnerValuesRange() {
    int iGasesNum = _config->getGasesNum();
    ImpulseVector& vImpulses = _config->getImpulse()->getVector();

    bool result = true;

    for (unsigned int gi = 0; gi < iGasesNum; gi++) {
        for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
            if (_value[gi][ii] < 0) {
                // wrong values
                result = false;
                std::cout << "Cell::checkInnerValuesRange(): Error [type][gi][ii]"
                          << "[" << Utils::asInteger(_computationType[0]) << ":" << Utils::asInteger(_computationType[1]) << ":" << Utils::asInteger(_computationType[2]) << "]"
                          << "[" << gi << "]"
                          << "[" << ii << "]"
                          << " value = " << _value[gi][ii] << std::endl;
            }
            if (_halfValue[gi][ii] < 0) {
                // wrong values
                result = false;
                std::cout << "Cell::checkInnerValuesRange(): Error [type][gi][ii]"
                          << "[" << Utils::asInteger(_computationType[0]) << ":" << Utils::asInteger(_computationType[1]) << ":" << Utils::asInteger(_computationType[2]) << "]"
                          << "[" << gi << "]"
                          << "[" << ii << "]"
                          << " half = " << _halfValue[gi][ii] << std::endl;
            }
        }
    }

    return result;
}

/* private */

// help methods
void Cell::compute_type(unsigned int dim) {
    if (_prev[dim] == nullptr) {
        if (_next[dim] != nullptr) {
            _computationType[dim] = ComputationType::LEFT;
        } else {
            //std::cout << "Cell has no neighbors in dim = " << dim << std::endl;
        }
    } else {
        if (_next[dim] == nullptr) {
            _computationType[dim] = ComputationType::RIGHT;
        } else {
            if (_next[dim]->_next[dim] == nullptr) {
                _computationType[dim] = ComputationType::PRE_RIGHT;
            } else {
                _computationType[dim] = ComputationType::NORMAL;
            }
        }
    }

    if (_computationType[dim] == ComputationType::UNDEFINED) {
        //std::cout << "Cannot find this cell type, maybe cells are not linked, or linked wrong." << std::endl;
    }
}

void Cell::compute_half_left(unsigned int dim) {
    int iGasesNum = _config->getGasesNum();

    for (int gi = 0; gi < iGasesNum; gi++) {
        switch (_data->getBoundaryTypes()[gi]) {
            case CellData::BoundaryType::DIFFUSE:
                compute_half_diffuse_left(dim, gi);
                break;
            case CellData::BoundaryType::PRESSURE:
                compute_half_gase_left(dim, gi);
                break;
            case CellData::BoundaryType::MIRROR:
                compute_half_mirror_left(dim, gi);
                break;
            case CellData::BoundaryType::FLOW:
                compute_half_flow_left(dim, gi);
                break;
        }
    }
}

void Cell::compute_half_normal(unsigned int dim) {
    const GasVector& vGases = _config->getGases();
    int iGasesNum = _config->getGasesNum();
    ImpulseVector& vImpulses = _config->getImpulse()->getVector();

    for (unsigned int gi = 0; gi < iGasesNum; gi++) {
        for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
            double y = _config->getTimestep() / vGases[gi]->getMass() * std::abs(vImpulses[ii][dim] / _data->getStep().get(dim));

            if (vImpulses[ii][dim] > 0) {
                _halfValue[gi][ii] = _value[gi][ii] + (1 - y) / 2 * limiter(
                        _prev[dim]->_value[gi][ii],
                        _value[gi][ii],
                        _next[dim]->_value[gi][ii]);
            } else {
                _halfValue[gi][ii] = _next[dim]->_value[gi][ii] - (1 - y) / 2 * limiter(
                        _value[gi][ii],
                        _next[dim]->_value[gi][ii],
                        _next[dim]->_next[dim]->_value[gi][ii]);
            }
        }
    }
}

void Cell::compute_half_preright(unsigned int dim) {}

void Cell::compute_half_right(unsigned int dim) {
    int iGasesNum = _config->getGasesNum();

    for (int gi = 0; gi < iGasesNum; gi++) {
        switch (_data->getBoundaryTypes()[gi]) {
            case CellData::BoundaryType::DIFFUSE:
                compute_half_diffuse_right(dim, gi);
                break;
            case CellData::BoundaryType::PRESSURE:
                compute_half_gase_right(dim, gi);
                break;
            case CellData::BoundaryType::MIRROR:
                compute_half_mirror_right(dim, gi);
                break;
            case CellData::BoundaryType::FLOW:
                compute_half_flow_right(dim, gi);
                break;
        }
    }
}

void Cell::compute_value_normal(unsigned int dim) {
    const GasVector& vGases = _config->getGases();
    int iGasesNum = _config->getGasesNum();
    ImpulseVector& vImpulses = _config->getImpulse()->getVector();

    for (unsigned int gi = 0; gi < iGasesNum; gi++) {
        for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
            double y = _config->getTimestep() / vGases[gi]->getMass() * vImpulses[ii][dim] / _data->getStep().get(dim);

            _value[gi][ii] = _value[gi][ii] - y * (_halfValue[gi][ii] - _prev[dim]->_halfValue[gi][ii]);
        }
    }
}

/* Diffuse boundary type. */
void Cell::compute_half_diffuse_left(unsigned int dim, int gi) {
    const GasVector& vGases = _config->getGases();
    ImpulseVector& vImpulses = _config->getImpulse()->getVector();

    double C1_up = 0.0;
    double C1_down = 0.0;
    double C2_up = 0.0;
    for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
        if (vImpulses[ii][dim] < 0) {
            double y = _config->getTimestep() / vGases[gi]->getMass() * std::abs(vImpulses[ii][dim] / _data->getStep().get(dim));

            _value[gi][ii] = 2 * _next[dim]->_value[gi][ii] - _next[dim]->_next[dim]->_value[gi][ii];
            if (_value[gi][ii] < 0)
                _value[gi][ii] = 0;

            _halfValue[gi][ii] = _next[dim]->_value[gi][ii] - (1 - y) / 2 * limiter(
                    _value[gi][ii],
                    _next[dim]->_value[gi][ii],
                    _next[dim]->_next[dim]->_value[gi][ii]);

            C1_up += std::abs(vImpulses[ii][dim] * _halfValue[gi][ii]);
            C2_up += std::abs(vImpulses[ii][dim] * (_value[gi][ii] + _next[dim]->_value[gi][ii]) / 2);
        } else {
            C1_down += std::abs(vImpulses[ii][dim] * compute_exp(vGases[gi]->getMass(), _data->getBoundaryParams(gi).getTemp(), vImpulses[ii]));
        }
    }

    for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
        if (vImpulses[ii][dim] > 0) {
            _halfValue[gi][ii] = C1_up / C1_down * compute_exp(vGases[gi]->getMass(), _data->getBoundaryParams(gi).getTemp(), vImpulses[ii]);
            _value[gi][ii] = 2 * C2_up / C1_down * compute_exp(vGases[gi]->getMass(), _data->getBoundaryParams(gi).getTemp(), vImpulses[ii]) - _next[dim]->_value[gi][ii];
            if (_value[gi][ii] < 0.0)
                _value[gi][ii] = 0.0;
        }
    }
}

void Cell::compute_half_diffuse_right(unsigned int dim, int gi) {
    const GasVector& vGases = _config->getGases();
    ImpulseVector& vImpulses = _config->getImpulse()->getVector();

    double C1_up = 0.0;
    double C1_down = 0.0;
    double C2_up = 0.0;
    for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
        if (vImpulses[ii][dim] > 0) {
            double y = _config->getTimestep() / vGases[gi]->getMass() * std::abs(vImpulses[ii][dim] / _data->getStep().get(dim));

            _value[gi][ii] = 2 * _prev[dim]->_value[gi][ii] - _prev[dim]->_prev[dim]->_value[gi][ii];
            if (_value[gi][ii] < 0)
                _value[gi][ii] = 0;

            _prev[dim]->_halfValue[gi][ii] = _prev[dim]->_value[gi][ii] + (1 - y) / 2 * limiter(
                    _prev[dim]->_prev[dim]->_value[gi][ii],
                    _prev[dim]->_value[gi][ii],
                    _value[gi][ii]);

            C1_up += std::abs(vImpulses[ii][dim] * _prev[dim]->_halfValue[gi][ii]);
            C2_up += std::abs(vImpulses[ii][dim] * (_value[gi][ii] + _prev[dim]->_value[gi][ii]) / 2);
        } else {
            C1_down += std::abs(vImpulses[ii][dim] * compute_exp(vGases[gi]->getMass(), _data->getBoundaryParams(gi).getTemp(), vImpulses[ii]));
        }
    }

    for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
        if (vImpulses[ii][dim] < 0) {
            _prev[dim]->_halfValue[gi][ii] = C1_up / C1_down * compute_exp(vGases[gi]->getMass(), _data->getBoundaryParams(gi).getTemp(), vImpulses[ii]);
            _value[gi][ii] = 2 * C2_up / C1_down * compute_exp(vGases[gi]->getMass(), _data->getBoundaryParams(gi).getTemp(), vImpulses[ii]) - _prev[dim]->_value[gi][ii];
            if (_value[gi][ii] < 0.0)
                _value[gi][ii] = 0.0;
        }
    }
}

/* Setting gases boundary type. */
void Cell::compute_half_gase_left(unsigned int dim, int gi) {
    const GasVector& vGases = _config->getGases();
    Impulse* pImpulse = _config->getImpulse();
    ImpulseVector& vImpulses = pImpulse->getVector();

    double C1_up = 0.0;
    double C1_down = 0.0;
    double C2_up = 0.0;

    Vector3d v3Speed = Vector3d();
    if (_data->getBoundaryParams(gi).getPressure() > 0.0) {
        v3Speed = _data->getBoundaryParams(gi).getFlow() / (_data->getBoundaryParams(gi).getPressure() / _data->getBoundaryParams(gi).getTemp());
    }

    for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
        if (vImpulses[ii][dim] < 0) {
            double y = _config->getTimestep() / vGases[gi]->getMass() * std::abs(vImpulses[ii][dim] / _data->getStep().get(dim));

            _value[gi][ii] = 2 * _next[dim]->_value[gi][ii] - _next[dim]->_next[dim]->_value[gi][ii];
            if (_value[gi][ii] < 0)
                _value[gi][ii] = 0;

            _halfValue[gi][ii] = _next[dim]->_value[gi][ii] - (1 - y) / 2 * limiter(
                    _value[gi][ii],
                    _next[dim]->_value[gi][ii],
                    _next[dim]->_next[dim]->_value[gi][ii]);

            C1_up += _halfValue[gi][ii];
            C2_up += (_value[gi][ii] + _next[dim]->_value[gi][ii]) / 2;
        } else {
            C1_down += compute_exp(
                    vGases[gi]->getMass(),
                    _data->getBoundaryParams(gi).getTemp(),
                    vImpulses[ii] - v3Speed * vGases[gi]->getMass());
        }
    }

    // Vacuum
    if (_data->getBoundaryParams(gi).getPressure() == 0.0) {
        C1_up = 0.0;
        C2_up = 0.0;
    } else {
        C1_up = _data->getBoundaryParams(gi).getPressure() / _data->getBoundaryParams(gi).getTemp() / pImpulse->getDeltaImpulseQube() - C1_up;
        C2_up = _data->getBoundaryParams(gi).getPressure() / _data->getBoundaryParams(gi).getTemp() / pImpulse->getDeltaImpulseQube() - C2_up;

        if (C1_up < 0.0)
            C1_up = 0.0;
        if (C2_up < 0.0)
            C2_up = 0.0;
    }

    for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
        if (vImpulses[ii][dim] > 0) {
            _halfValue[gi][ii] = C1_up / C1_down * compute_exp(
                    vGases[gi]->getMass(),
                    _data->getBoundaryParams(gi).getTemp(),
                    vImpulses[ii] - v3Speed * vGases[gi]->getMass());

            _value[gi][ii] = 2 * C2_up / C1_down * compute_exp(
                    vGases[gi]->getMass(),
                    _data->getBoundaryParams(gi).getTemp(),
                    vImpulses[ii] - v3Speed * vGases[gi]->getMass()) - _next[dim]->_value[gi][ii];
            if (_value[gi][ii] < 0.0)
                _value[gi][ii] = 0.0;
        }
    }
}

void Cell::compute_half_gase_right(unsigned int dim, int gi) {
    const GasVector& vGases = _config->getGases();
    Impulse* pImpulse = _config->getImpulse();
    ImpulseVector& vImpulses = pImpulse->getVector();

    double C1_up = 0.0;
    double C1_down = 0.0;
    double C2_up = 0.0;

    Vector3d v3Speed = Vector3d();
    if (_data->getBoundaryParams(gi).getPressure() > 0.0) {
        v3Speed = _data->getBoundaryParams(gi).getFlow() / (_data->getBoundaryParams(gi).getPressure() / _data->getBoundaryParams(gi).getTemp());
    }

    for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
        if (vImpulses[ii][dim] > 0) {
            double y = _config->getTimestep() / vGases[gi]->getMass() * std::abs(vImpulses[ii][dim] / _data->getStep().get(dim));

            _value[gi][ii] = 2 * _prev[dim]->_value[gi][ii] - _prev[dim]->_prev[dim]->_value[gi][ii];
            if (_value[gi][ii] < 0)
                _value[gi][ii] = 0;

            _prev[dim]->_halfValue[gi][ii] = _prev[dim]->_value[gi][ii] + (1 - y) / 2 * limiter(
                    _prev[dim]->_prev[dim]->_value[gi][ii],
                    _prev[dim]->_value[gi][ii],
                    _value[gi][ii]);

            C1_up += _prev[dim]->_halfValue[gi][ii];
            C2_up += (_value[gi][ii] + _prev[dim]->_value[gi][ii]) / 2;
        } else {
            C1_down += compute_exp(
                    vGases[gi]->getMass(),
                    _data->getBoundaryParams(gi).getTemp(),
                    vImpulses[ii] - v3Speed * vGases[gi]->getMass());
        }
    }

    // Vacuum
    if (_data->getBoundaryParams(gi).getPressure() == 0.0) {
        C1_up = 0.0;
        C2_up = 0.0;
    } else {
        C1_up = _data->getBoundaryParams(gi).getPressure() / _data->getBoundaryParams(gi).getTemp() / pImpulse->getDeltaImpulseQube() - C1_up;
        C2_up = _data->getBoundaryParams(gi).getPressure() / _data->getBoundaryParams(gi).getTemp() / pImpulse->getDeltaImpulseQube() - C2_up;

        if (C1_up < 0.0)
            C1_up = 0.0;
        if (C2_up < 0.0)
            C2_up = 0.0;
    }

    for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
        if (vImpulses[ii][dim] < 0) {
            _prev[dim]->_halfValue[gi][ii] = C1_up / C1_down * compute_exp(
                    vGases[gi]->getMass(),
                    _data->getBoundaryParams(gi).getTemp(),
                    vImpulses[ii] - v3Speed * vGases[gi]->getMass());

            _value[gi][ii] = 2 * C2_up / C1_down * compute_exp(
                    vGases[gi]->getMass(),
                    _data->getBoundaryParams(gi).getTemp(),
                    vImpulses[ii] - v3Speed * vGases[gi]->getMass()) - _prev[dim]->_value[gi][ii];
            if (_value[gi][ii] < 0.0)
                _value[gi][ii] = 0.0;
        }
    }
}

/* Setting flow boundary type. */
void Cell::compute_half_flow_left(unsigned int dim, int gi) {
    const GasVector& vGases = _config->getGases();
    Impulse* pImpulse = _config->getImpulse();
    ImpulseVector& vImpulses = pImpulse->getVector();

    double C1_up = 0.0;
    double C1_down = 0.0;
    double C2_up = 0.0;

    for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
        if (vImpulses[ii][dim] < 0) {
            double y = _config->getTimestep() / vGases[gi]->getMass() * std::abs(vImpulses[ii][dim] / _data->getStep().get(dim));

            _value[gi][ii] = 2 * _next[dim]->_value[gi][ii] - _next[dim]->_next[dim]->_value[gi][ii];
            if (_value[gi][ii] < 0)
                _value[gi][ii] = 0;

            _halfValue[gi][ii] = _next[dim]->_value[gi][ii] - (1 - y) / 2 * limiter(
                    _value[gi][ii],
                    _next[dim]->_value[gi][ii],
                    _next[dim]->_next[dim]->_value[gi][ii]);

            C1_up += vImpulses[ii][dim] * _halfValue[gi][ii];
            C2_up += vImpulses[ii][dim] * (_value[gi][ii] + _next[dim]->_value[gi][ii]) / 2;
        } else {
            C1_down += vImpulses[ii][dim] * compute_exp(
                    vGases[gi]->getMass(),
                    _data->getBoundaryParams(gi).getTemp(),
                    vImpulses[ii]);
        }
    }

    C1_up = _data->getBoundaryParams(gi).getFlow().get(dim) / pImpulse->getDeltaImpulseQube() - C1_up;
    C2_up = _data->getBoundaryParams(gi).getFlow().get(dim) / pImpulse->getDeltaImpulseQube() - C2_up;

    if (C1_up / C1_down < 0.0) {
        C1_up = 0.0;
    }
    if (C2_up / C1_down < 0.0) {
        C2_up = 0.0;
    }

    for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
        if (vImpulses[ii][dim] > 0) {
            _halfValue[gi][ii] = C1_up / C1_down * compute_exp(
                    vGases[gi]->getMass(),
                    _data->getBoundaryParams(gi).getTemp(),
                    vImpulses[ii]);

            _value[gi][ii] = 2 * C2_up / C1_down * compute_exp(
                    vGases[gi]->getMass(),
                    _data->getBoundaryParams(gi).getTemp(),
                    vImpulses[ii]) - _next[dim]->_value[gi][ii];

            if (_value[gi][ii] < 0.0) {
                _value[gi][ii] = 0.0;
            }
        }
    }
}

void Cell::compute_half_flow_right(unsigned int dim, int gi) {
    const GasVector& vGases = _config->getGases();
    Impulse* pImpulse = _config->getImpulse();
    ImpulseVector& vImpulses = pImpulse->getVector();

    double C1_up = 0.0;
    double C1_down = 0.0;
    double C2_up = 0.0;

    for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
        if (vImpulses[ii][dim] > 0) {
            double y = _config->getTimestep() / vGases[gi]->getMass() * std::abs(vImpulses[ii][dim] / _data->getStep().get(dim));

            _value[gi][ii] = 2 * _prev[dim]->_value[gi][ii] - _prev[dim]->_prev[dim]->_value[gi][ii];
            if (_value[gi][ii] < 0)
                _value[gi][ii] = 0;

            _prev[dim]->_halfValue[gi][ii] = _prev[dim]->_value[gi][ii] + (1 - y) / 2 * limiter(
                    _prev[dim]->_prev[dim]->_value[gi][ii],
                    _prev[dim]->_value[gi][ii],
                    _value[gi][ii]);

            C1_up += std::abs(vImpulses[ii][dim]) * _prev[dim]->_halfValue[gi][ii];
            C2_up += std::abs(vImpulses[ii][dim]) * (_value[gi][ii] + _prev[dim]->_value[gi][ii]) / 2;
        } else {
            C1_down += std::abs(vImpulses[ii][dim]) * compute_exp(
                    vGases[gi]->getMass(),
                    _data->getBoundaryParams(gi).getTemp(),
                    vImpulses[ii]);
        }
    }

    C1_up = _data->getBoundaryParams(gi).getFlow().get(dim) / pImpulse->getDeltaImpulseQube() - C1_up;
    C2_up = _data->getBoundaryParams(gi).getFlow().get(dim) / pImpulse->getDeltaImpulseQube() - C2_up;

    if (C1_up / C1_down < 0.0)
        C1_up = 0.0;
    if (C2_up / C1_down < 0.0)
        C2_up = 0.0;

    for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
        if (vImpulses[ii][dim] < 0) {
            _prev[dim]->_halfValue[gi][ii] = C1_up / C1_down * compute_exp(
                    vGases[gi]->getMass(),
                    _data->getBoundaryParams(gi).getTemp(),
                    vImpulses[ii]);

            _value[gi][ii] = 2 * C2_up / C1_down * compute_exp(
                    vGases[gi]->getMass(),
                    _data->getBoundaryParams(gi).getTemp(),
                    vImpulses[ii]) - _prev[dim]->_value[gi][ii];
            if (_value[gi][ii] < 0.0)
                _value[gi][ii] = 0.0;
        }
    }
}

/* Setting mirror boundary type. */
void Cell::compute_half_mirror_left(unsigned int dim, int gi) {
    const GasVector& vGases = _config->getGases();
    Impulse* pImpulse = _config->getImpulse();
    ImpulseVector& vImpulses = pImpulse->getVector();

    for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
        double y = _config->getTimestep() / vGases[gi]->getMass() * std::abs(vImpulses[ii][dim] / _data->getStep().get(dim));

        if (vImpulses[ii][dim] > 0) {
            int ri = pImpulse->reverseIndex(ii, static_cast<sep::Axis>(dim));

            _halfValue[gi][ii] = _value[gi][ii] + (1 - y) / 2 * limiter(
                    _value[gi][ri], // reversed for left
                    _value[gi][ii],
                    _next[dim]->_value[gi][ii]);
        } else {
            _halfValue[gi][ii] = _next[dim]->_value[gi][ii] - (1 - y) / 2 * limiter(
                    _value[gi][ii],
                    _next[dim]->_value[gi][ii],
                    _next[dim]->_next[dim]->_value[gi][ii]);
        }
    }

    for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
        double y = _config->getTimestep() / vGases[gi]->getMass() * vImpulses[ii][dim] / _data->getStep().get(dim);
        int ri = pImpulse->reverseIndex(ii, static_cast<sep::Axis>(dim));
        _value[gi][ii] = _value[gi][ii] - y * (_halfValue[gi][ii] - _halfValue[gi][ri]);
    }
}

void Cell::compute_half_mirror_right(unsigned int dim, int gi) {
    const GasVector& vGases = _config->getGases();
    Impulse* pImpulse = _config->getImpulse();
    ImpulseVector& vImpulses = pImpulse->getVector();

    for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
        double y = _config->getTimestep() / vGases[gi]->getMass() * std::abs(vImpulses[ii][dim] / _data->getStep().get(dim));

        int ri = pImpulse->reverseIndex(ii, static_cast<sep::Axis>(dim));

        if (vImpulses[ii][dim] > 0) {
            // PREV
            _prev[dim]->_halfValue[gi][ii] = _prev[dim]->_value[gi][ii] + (1 - y) / 2 * limiter(
                    _prev[dim]->_prev[dim]->_value[gi][ii],
                    _prev[dim]->_value[gi][ii],
                    _value[gi][ii]);

            // CURRENT
            _halfValue[gi][ii] = _value[gi][ii] + (1 - y) / 2 * limiter(
                    _prev[dim]->_value[gi][ii],
                    _value[gi][ii],
                    _value[gi][ri]); // reversed for right
        } else {
            // PREV
            _prev[dim]->_halfValue[gi][ii] = _value[gi][ii] - (1 - y) / 2 * limiter(
                    _prev[dim]->_value[gi][ii],
                    _value[gi][ii],
                    _value[gi][ri]);

            // CURRENT
            _halfValue[gi][ii] = _value[gi][ri] - (1 - y) / 2 * limiter(
                    _value[gi][ii],
                    _value[gi][ri],
                    _prev[dim]->_value[gi][ii]);
        }
    }

    for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
        double y = _config->getTimestep() / vGases[gi]->getMass() * vImpulses[ii][dim] / _data->getStep().get(dim);
        _value[gi][ii] = _value[gi][ii] - y * (_halfValue[gi][ii] - _prev[dim]->_halfValue[gi][ii]);
    }
}

double Cell::compute_exp(const double& mass, const double& temp, const Vector3d& impulse) {
    return std::exp(-impulse.mod2() / mass / 2 / temp);
}

double Cell::limiter_superbee(const double& x, const double& y, const double& z) {
    if ((z - y) * (y - x) <= 0) {
        return 0.0;
    } else {
        return std::max(0.0, std::min(2 * std::abs(y - x), std::min(std::abs(z - y), std::min(std::abs(y - x), 2 * std::abs(z - y))))) * Utils::sgn(z - y);
    }
}

// Macro Data =========================================================================================================

double Cell::compute_concentration(unsigned int gi) {
    Impulse* pImpulse = _config->getImpulse();
    ImpulseVector& vImpulses = pImpulse->getVector();

    double concentration = 0.0;
    for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
        concentration += _value[gi][ii];
    }
    concentration *= pImpulse->getDeltaImpulseQube();
    return concentration;
}

double Cell::compute_temperature(unsigned int gi, double density, const Vector3d& stream) {
    const GasVector& gasv = _config->getGases();
    Impulse* impulse = _config->getImpulse();
    ImpulseVector& impulsev = impulse->getVector();

    double concentration = density;
    double temperature = 0.0;
    Vector3d vAverageSpeed = stream;
    vAverageSpeed /= concentration;

    for (unsigned int ii = 0; ii < impulsev.size(); ii++) {
        Vector3d tempvec;
        for (unsigned int vi = 0; vi < vAverageSpeed.getMass().size(); vi++) {
            tempvec[vi] = impulsev[ii][vi] / gasv[gi]->getMass() - vAverageSpeed[vi];
        }
        temperature += gasv[gi]->getMass() * tempvec.mod2() * _value[gi][ii];
    }
    //cout << uvec[0] << ":" << uvec[1] << ":" << uvec[2] << endl;
    temperature *= impulse->getDeltaImpulseQube() / concentration / 3;

    //std::cout << temperature << std::endl;
    return temperature;
}

double Cell::compute_pressure(unsigned int gi, double density, double temperature) {
    return density * temperature;
}

Vector3d Cell::compute_stream(unsigned int gi) {
    const GasVector& vGases = _config->getGases();
    Impulse* pImpulse = _config->getImpulse();
    ImpulseVector& vImpulses = pImpulse->getVector();

    Vector3d vStream;
    for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
        for (unsigned int vi = 0; vi < vStream.getMass().size(); vi++) {
            vStream[vi] += vImpulses[ii][vi] * _value[gi][ii];
        }
    }
    vStream *= pImpulse->getDeltaImpulseQube() / vGases[gi]->getMass();

    return vStream;
}

Vector3d Cell::compute_heatstream(unsigned int gi) {
    return Vector3d();
}