#include "Cell.h"
#include "CellConnection.h"
#include "parameters/Gas.h"
#include "parameters/Impulse.h"
#include "integral/ci.hpp"

#include <boost/format.hpp>

Cell::Cell(int id, Cell::Type type, double volume) : _id(id), _type(type), _volume(volume) {}

void Cell::init() {
    auto config = Config::getInstance();
    const auto& gases = config->getGases();
    const auto& impulses = config->getImpulse()->getVector();
    auto gasesCount = config->getGasesCount();

    // Allocating space for values and new values
    _values.resize(gasesCount);
    _newValues.resize(gasesCount);

    for (unsigned int gi = 0; gi < gasesCount; gi++) {
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

int Cell::getId() const {
    return _id;
}

Cell::Type Cell::getType() const {
    return _type;
}

std::vector<DoubleVector>& Cell::getValues() {
    return _values;
}

const std::vector<std::shared_ptr<CellConnection>>& Cell::getConnections() const {
    return _connections;
}

double Cell::getVolume() const {
    return _volume;
}

void Cell::addConnection(CellConnection* connection) {
    _connections.emplace_back(connection);
}

void Cell::computeTransfer() {
    switch (_type) {
        case Type::NORMAL:
            compute_transfer_normal();
            break;
        case Type::BORDER:
            compute_transfer_border();
            break;
        case Type::PARALLEL:
            break;
    }
}

void Cell::swapValues() {
    auto config = Config::getInstance();
    const auto& impulses = config->getImpulse()->getVector();
    auto gasesCount = config->getGasesCount();
    for (unsigned int gi = 0; gi < gasesCount; gi++) {
        for (unsigned int ii = 0; ii < impulses.size(); ii++) {
            _values[gi][ii] = _newValues[gi][ii];
            _newValues[gi][ii] = 0.0;
        }
    }
}

void Cell::computeIntegral(unsigned int gi0, unsigned int gi1) {
    ci::iter(_values[gi0], _values[gi1]);
}

void Cell::computeBetaDecay(unsigned int gi0, unsigned int gi1, double lambda) {
    Config* config = Config::getInstance();
    const std::vector<Vector3d>& vImpulse = config->getImpulse()->getVector();

    for (unsigned int ii = 0; ii < vImpulse.size(); ii++) {
        double impact = _values[gi0][ii] * lambda * config->getTimestep(); // TODO: check lambda * Config::m_dTimestep < 1 !!!
        _values[gi0][ii] -= impact;
        _values[gi1][ii] += impact;
    }
}

void Cell::check() {
    auto config = Config::getInstance();
    const auto& impulses = config->getImpulse()->getVector();
    auto gasesCount = config->getGasesCount();
    for (unsigned int gi = 0; gi < gasesCount; gi++) {
        for (unsigned int ii = 0; ii < impulses.size(); ii++) {
            if (_values[gi][ii] < 0.0) {
                std::string text = (boost::format("Values below zero: gi = %d; ii = %d; value = %f") % gi % ii % _values[gi][ii]).str();
                throw std::runtime_error(text);
            }
        }
    }
}

CellParameters& Cell::getParams() {
    return _params;
}

CellParameters& Cell::getBoundaryParams() {
    return _boundaryParams;
}

CellResults* Cell::getResults() {

    // lazy initialization
    if (_results == nullptr) {
        _results.reset(new CellResults(_id));
    }

    // clear current values
    _results->reset();
    if (_type == Type::NORMAL) {
        for (unsigned int gi = 0; gi < Config::getInstance()->getGasesCount(); gi++) {
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
    }

    return _results.get();
}

// private -------------------------------------------------------------------------------------------------------------------------------------------
inline double Cell::fast_exp(const double& mass, const double& temp, const Vector3d& impulse) {
    return std::exp(-impulse.moduleSquare() / mass / 2 / temp);
}

void Cell::compute_transfer_normal() {
    auto config = Config::getInstance();
    const auto& gases = config->getGases();
    const auto& impulses = config->getImpulse()->getVector();
    auto gasesCount = config->getGasesCount();

    for (unsigned int gi = 0; gi < gasesCount; gi++) {
        for (unsigned int ii = 0; ii < impulses.size(); ii++) {
            double sum = 0.0;
            for (const auto& connection : _connections) {
                sum += connection->getValue(gi, ii, impulses[ii]);
            }
            _newValues[gi][ii] = _values[gi][ii] + sum * config->getTimestep() / _volume / gases[gi].getMass();
        }
    }
}

void Cell::compute_transfer_border() {
    if (_connections.size() != 1) {
        throw std::runtime_error("wrong border connection");
    }
    const auto& connection = _connections[0];

    auto config = Config::getInstance();
    const auto& gases = config->getGases();
    const auto& impulses = config->getImpulse()->getVector();
    auto gasesCount = config->getGasesCount();

    double cUp = 0.0, cDown = 0.0;
    for (unsigned int gi = 0; gi < gasesCount; gi++) {
        for (unsigned int ii = 0; ii < impulses.size(); ii++) {
            double projection = impulses[ii].scalar(connection->getNormal12());
            if (projection > 0.0) {
                cUp += projection * connection->getSecond()->getValues()[gi][ii];
            } else {
                cDown += -projection * fast_exp(gases[gi].getMass(), _boundaryParams.getTemp(gi), impulses[ii]);
            }
        }
    }

    double h = cUp / cDown;
    for (unsigned int gi = 0; gi < gasesCount; gi++) {
        for (unsigned int ii = 0; ii < impulses.size(); ii++) {
            double projection = impulses[ii].scalar(connection->getNormal12());
            if (projection < 0.0) {
                _values[gi][ii] = h * fast_exp(gases[gi].getMass(), _boundaryParams.getTemp(gi), impulses[ii]);
            }
        }
    }
}

// Macro Data =========================================================================================================

double Cell::compute_density(unsigned int gi) {
    Impulse* pImpulse = Config::getInstance()->getImpulse();
    const std::vector<Vector3d>& vImpulses = pImpulse->getVector();

    double concentration = 0.0;
    for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
        concentration += _values[gi][ii];
    }
    concentration *= pImpulse->getDeltaImpulseQube();
    return concentration;
}

double Cell::compute_temperature(unsigned int gi, double density, const Vector3d& stream) {
    const auto& vGases = Config::getInstance()->getGases();
    auto pImpulse = Config::getInstance()->getImpulse();
    const auto& vImpulses = pImpulse->getVector();

    double temperature = 0.0;
    Vector3d vAverageSpeed = stream;
    vAverageSpeed /= density;

    for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
        Vector3d vTemp;
        for (unsigned int vi = 0; vi < vAverageSpeed.getArray().size(); vi++) {
            vTemp[vi] = vImpulses[ii].get(vi) / vGases[gi].getMass() - vAverageSpeed[vi];
        }
        temperature += vGases[gi].getMass() * vTemp.moduleSquare() * _values[gi][ii];
    }
    temperature *= pImpulse->getDeltaImpulseQube() / density / 3;

    return temperature;
}

double Cell::compute_pressure(unsigned int gi, double density, double temperature) {
    return density * temperature;
}

Vector3d Cell::compute_stream(unsigned int gi) {
    const auto& vGases = Config::getInstance()->getGases();
    auto pImpulse = Config::getInstance()->getImpulse();
    const auto& vImpulses = pImpulse->getVector();

    Vector3d vStream;
    for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
        for (unsigned int vi = 0; vi < vStream.getArray().size(); vi++) {
            vStream[vi] += vImpulses[ii].get(vi) * _values[gi][ii];
        }
    }
    vStream *= pImpulse->getDeltaImpulseQube() / vGases[gi].getMass();

    return vStream;
}

Vector3d Cell::compute_heatstream(unsigned int gi) {
    return Vector3d();
}
