#include "NormalCell.h"
#include "CellConnection.h"
#include "integral/ci.hpp"

NormalCell::NormalCell(int id, double volume) : BaseCell(Type::NORMAL, id) {
    _volume = volume;
}

void NormalCell::init() {
    auto config = Config::getInstance();
    const auto& gases = config->getGases();
    const auto& impulses = config->getImpulse()->getVector();

    // Allocating space for values and new values
    _values.resize(gases.size());
    _newValues.resize(gases.size());

    for (unsigned int gi = 0; gi < gases.size(); gi++) {
        _values[gi].resize(impulses.size(), 0.0);
        _newValues[gi].resize(impulses.size(), 0.0);

        double dDensity = 0.0;
        for (const auto& impulse : impulses) {
            dDensity += fast_exp(gases[gi].getMass(), _params.getTemp(gi), impulse);
        }

        dDensity *= config->getImpulse()->getDeltaImpulseQube();
        dDensity = _params.getPressure(gi) / _params.getTemp(gi) / dDensity;

        for (unsigned int ii = 0; ii < impulses.size(); ii++) {
            _values[gi][ii] = dDensity * fast_exp(
                    gases[gi].getMass(),
                    _params.getTemp(gi),
                    impulses[ii]
            );
        }
    }
}

void NormalCell::computeTransfer() {
    auto config = Config::getInstance();
    const auto& gases = config->getGases();
    const auto& impulses = config->getImpulse()->getVector();

    for (unsigned int gi = 0; gi < gases.size(); gi++) {
        for (unsigned int ii = 0; ii < impulses.size(); ii++) {
            double sum = 0.0;
            for (const auto& connection : _connections) {
                sum += connection->getValue(gi, ii, impulses[ii]);
            }
            _newValues[gi][ii] = _values[gi][ii] + sum * config->getTimestep() / _volume / gases[gi].getMass();
        }
    }
}

void NormalCell::computeIntegral(unsigned int gi0, unsigned int gi1) {
    ci::iter(_values[gi0], _values[gi1]);
}

void NormalCell::computeBetaDecay(unsigned int gi0, unsigned int gi1, double lambda) {
    Config* config = Config::getInstance();
    const std::vector<Vector3d>& vImpulse = config->getImpulse()->getVector();

    for (unsigned int ii = 0; ii < vImpulse.size(); ii++) {
        double impact = _values[gi0][ii] * lambda * config->getTimestep(); // TODO: check lambda * Config::m_dTimestep < 1 !!!
        _values[gi0][ii] -= impact;
        _values[gi1][ii] += impact;
    }
}

double NormalCell::getVolume() const {
    return _volume;
}

CellParameters& NormalCell::getParams() {
    return _params;
}

void NormalCell::swapValues() {
    auto config = Config::getInstance();
    const auto& gases = config->getGases();
    const auto& impulses = config->getImpulse()->getVector();

    for (unsigned int gi = 0; gi < gases.size(); gi++) {
        for (unsigned int ii = 0; ii < impulses.size(); ii++) {
            _values[gi][ii] = _newValues[gi][ii];
            _newValues[gi][ii] = 0.0;
        }
    }
}

CellResults* NormalCell::getResults() {
    auto config = Config::getInstance();
    const auto& gases = config->getGases();

    // lazy initialization
    if (_results == nullptr) {
        _results.reset(new CellResults(_id));
    }

    // clear current values
    _results->reset();

    // fill results
    for (unsigned int gi = 0; gi < gases.size(); gi++) {
        double density = compute_density(gi);
        Vector3d flow = compute_stream(gi);

        double temp = 0.0, pressure = 0.0;
        Vector3d heatFlow;

        if (density > 0.0) {
            temp = compute_temperature(gi, density, flow);
            pressure = compute_pressure(gi, density, temp);
            heatFlow = compute_heatstream(gi);
        }

        _results->set(gi, pressure, density, temp, flow, heatFlow);
    }

    return _results.get();
}

double NormalCell::compute_density(unsigned int gi) {
    auto config = Config::getInstance();
    const auto& gases = config->getGases();
    auto impulse = config->getImpulse();
    const auto& impulses = config->getImpulse()->getVector();

    double concentration = 0.0;
    for (unsigned int ii = 0; ii < impulses.size(); ii++) {
        concentration += _values[gi][ii];
    }
    concentration *= impulse->getDeltaImpulseQube();
    return concentration;
}

double NormalCell::compute_temperature(unsigned int gi, double density, const Vector3d& stream) {
    auto config = Config::getInstance();
    const auto& gases = config->getGases();
    auto impulse = config->getImpulse();
    const auto& impulses = impulse->getVector();

    double temperature = 0.0;
    Vector3d vAverageSpeed = stream;
    vAverageSpeed /= density;

    for (unsigned int ii = 0; ii < impulses.size(); ii++) {
        Vector3d vTemp;
        for (unsigned int vi = 0; vi < vAverageSpeed.getArray().size(); vi++) {
            vTemp[vi] = impulses[ii].get(vi) / gases[gi].getMass() - vAverageSpeed[vi];
        }
        temperature += gases[gi].getMass() * vTemp.moduleSquare() * _values[gi][ii];
    }
    temperature *= impulse->getDeltaImpulseQube() / density / 3;

    return temperature;
}

double NormalCell::compute_pressure(unsigned int gi, double density, double temperature) {
    return density * temperature;
}

Vector3d NormalCell::compute_stream(unsigned int gi) {
    auto config = Config::getInstance();
    const auto& gases = config->getGases();
    auto impulse = config->getImpulse();
    const auto& impulses = impulse->getVector();

    Vector3d vStream;
    for (unsigned int ii = 0; ii < impulses.size(); ii++) {
        for (unsigned int vi = 0; vi < vStream.getArray().size(); vi++) {
            vStream[vi] += impulses[ii].get(vi) * _values[gi][ii];
        }
    }
    vStream *= impulse->getDeltaImpulseQube() / gases[gi].getMass();

    return vStream;
}

Vector3d NormalCell::compute_heatstream(unsigned int gi) {
    return Vector3d();
}
