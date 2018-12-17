#include "BaseCell.h"
#include "CellConnection.h"
#include "parameters/ImpulseSphere.h"
#include "parameters/Gas.h"
#include "core/Config.h"
#include "utilities/Utils.h"

#include <cmath>
#include <boost/format.hpp>

BaseCell::BaseCell(Type type, int id) : _type(type), _id(id) {}

int BaseCell::getId() const {
    return _id;
}

BaseCell::Type BaseCell::getType() const {
    return _type;
}

std::vector<std::vector<double>>& BaseCell::getValues() {
    return _values;
}

const std::vector<std::shared_ptr<CellConnection>>& BaseCell::getConnections() const {
    return _connections;
}

void BaseCell::addConnection(CellConnection* connection) {
    _connections.emplace_back(connection);
}

void BaseCell::check() {
    auto config = Config::getInstance();
    const auto& gases = config->getGases();
    const auto& impulses = config->getImpulseSphere()->getImpulses();

    for (unsigned int gi = 0; gi < gases.size(); gi++) {
        for (unsigned int ii = 0; ii < impulses.size(); ii++) {
            if (_values[gi][ii] < -0.0) {
                std::string text = (boost::format("Values below zero: gi = %d; ii = %d; value = %f; id = %d; type = %d")
                                    % gi % ii % _values[gi][ii] % _id % Utils::asNumber(_type)).str();
                throw std::runtime_error(text);
            }
        }
    }
}

double BaseCell::fast_exp(double mass, double temp, const Vector3d& impulse) {
    return std::exp(-impulse.moduleSquare() / mass / 2 / temp);
}
