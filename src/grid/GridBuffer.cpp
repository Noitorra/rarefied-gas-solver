#include "GridBuffer.h"
#include "core/Config.h"
#include "utilities/Parallel.h"
#include "utilities/SerializationUtils.h"

void GridBuffer::calculateAverage() {
    if (Parallel::isSingle() == false) {
        if (Parallel::isMaster() == true) {

            // gather all values
            for (int processor = 1; processor < Parallel::getSize(); processor++) {
                std::vector<CellResults*> results;
                SerializationUtils::deserialize(Parallel::recv(processor, Parallel::COMMAND_BUFFER_RESULTS), results);

                _allResults.insert(_allResults.end(), results.begin(), results.end());
            }

            // get average
            calculateAverageSingle();

            // send average to other nodes
            for (int processor = 1; processor < Parallel::getSize(); processor++) {
                Parallel::send(SerializationUtils::serialize(_averageResults), processor, Parallel::COMMAND_BUFFER_AVERAGE_RESULTS);
            }
        } else {
            Parallel::send(SerializationUtils::serialize(_allResults), 0, Parallel::COMMAND_BUFFER_RESULTS);
            SerializationUtils::deserialize(Parallel::recv(0, Parallel::COMMAND_BUFFER_AVERAGE_RESULTS), _averageResults);
        }
    } else {
        calculateAverageSingle();
    }
}

void GridBuffer::calculateAverageSingle() {
    auto config = Config::getInstance();
    const auto& gases = config->getGases();

    for (unsigned int gi = 0; gi < gases.size(); gi++) {
        _averageResults.set(gi, 0, 0, 0, Vector3d(), Vector3d());
    }

    auto n = _allResults.size();
    if (n > 0) {
        for (auto results : _allResults) {
            for (unsigned int gi = 0; gi < gases.size(); gi++) {
                _averageResults.setPressure(gi, _averageResults.getPressure(gi) + results->getPressure(gi));
                _averageResults.setDensity(gi, _averageResults.getDensity(gi) + results->getDensity(gi));
                _averageResults.setTemp(gi, _averageResults.getTemp(gi) + results->getTemp(gi));
                _averageResults.setFlow(gi, _averageResults.getFlow(gi) + results->getFlow(gi));
                _averageResults.setHeatFlow(gi, _averageResults.getHeatFlow(gi) + results->getHeatFlow(gi));
            }
        }
        for (unsigned int gi = 0; gi < gases.size(); gi++) {
            _averageResults.setPressure(gi, _averageResults.getPressure(gi) / n);
            _averageResults.setDensity(gi, _averageResults.getDensity(gi) / n);
            _averageResults.setTemp(gi, _averageResults.getTemp(gi) / n);
            _averageResults.setFlow(gi, _averageResults.getFlow(gi) / n);
            _averageResults.setHeatFlow(gi, _averageResults.getHeatFlow(gi) / n);
        }
    }
}


