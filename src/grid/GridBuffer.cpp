#include "GridBuffer.h"
#include "core/Config.h"
#include "utilities/Parallel.h"
#include "utilities/SerializationUtils.h"

void GridBuffer::calculateAverageFlow() {
    if (Parallel::isSingle() == false) {
        if (Parallel::isMaster() == true) {

            // gather all values
            for (int processor = 1; processor < Parallel::getSize(); processor++) {
                std::map<std::string, std::vector<std::vector<double>>> flows;
                SerializationUtils::deserialize(Parallel::recv(processor, Parallel::COMMAND_BUFFER_RESULTS), flows);

                for (const auto& item : flows) {
                    const auto& gases = Config::getInstance()->getGases();
                    if (_allFlows.count(item.first) == 0) {
                        std::vector<std::vector<double>> newFlows(gases.size());
                        _allFlows[item.first] = newFlows;
                    }
                    for (unsigned int gi = 0; gi < gases.size(); gi++) {
                        _allFlows[item.first][gi].insert(_allFlows[item.first][gi].end(), item.second[gi].begin(), item.second[gi].end());
                    }
                }
            }

            // get average
            calculateAverageFlowSingle();

            // send average to other nodes
            for (int processor = 1; processor < Parallel::getSize(); processor++) {
                Parallel::send(SerializationUtils::serialize(_averageFlow), processor, Parallel::COMMAND_BUFFER_AVERAGE_RESULTS);
            }
        } else {
            Parallel::send(SerializationUtils::serialize(_allFlows), 0, Parallel::COMMAND_BUFFER_RESULTS);
            SerializationUtils::deserialize(Parallel::recv(0, Parallel::COMMAND_BUFFER_AVERAGE_RESULTS), _averageFlow);
        }
    } else {
        calculateAverageFlowSingle();
    }
}

void GridBuffer::calculateAverageFlowSingle() {
    const auto& gases = Config::getInstance()->getGases();

    for (auto& flows : _allFlows) {
        std::vector<double> newFlows(gases.size());

        for (unsigned int gi = 0; gi < gases.size(); gi++) {
            newFlows[gi] = 0.0;

            for (auto flow : flows.second[gi]) {
                newFlows[gi] += flow;
            }

            newFlows[gi] /= flows.second[gi].size();
        }

        _averageFlow[flows.first] = newFlows;
    }
}




