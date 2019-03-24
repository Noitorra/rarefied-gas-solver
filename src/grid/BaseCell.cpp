#include "BaseCell.h"
#include "CellConnection.h"
#include "parameters/ImpulseSphere.h"
#include "parameters/Gas.h"
#include "core/Config.h"
#include "utilities/Utils.h"

#include <cmath>
#include <boost/format.hpp>
#include <stdexcept>

void BaseCell::check() {
    auto config = Config::getInstance();
    const auto& gases = config->getGases();
    const auto& impulses = config->getImpulseSphere()->getImpulses();

    for (unsigned int gi = 0; gi < gases.size(); gi++) {
        for (unsigned int ii = 0; ii < impulses.size(); ii++) {
            if (_values[gi][ii] < -0.1) {
                std::string text = (boost::format("Values below zero: gi = %d; ii = %d; value = %f; id = %d; type = %d")
                                    % gi % ii % _values[gi][ii] % _id % Utils::asNumber(_type)).str();
                throw std::runtime_error(text);
            }
        }
    }
}

void BaseCell::addConnection(CellConnection* connection) {
    _connections.emplace_back(connection);
}
