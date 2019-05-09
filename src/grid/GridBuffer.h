#ifndef RGS_GRIDBUFFER_H
#define RGS_GRIDBUFFER_H

#include "core/Config.h"

#include <vector>
#include <map>
#include <iostream>

class GridBuffer {
private:
    std::map<std::string, std::vector<double>> _averageFlow;
    std::map<std::string, std::vector<std::vector<double>>> _allFlows;

public:
    GridBuffer() = default;

    void clearAllFlows() {
        _allFlows.clear();
    }

    void addFlow(const std::string& group, unsigned int gi, double flow) {
        if (_allFlows.count(group) == 0) {
            const auto& gases = Config::getInstance()->getGases();

            std::vector<std::vector<double>> flows(gases.size());
            _allFlows[group] = flows;
        }
        _allFlows[group][gi].push_back(flow);
    }

    double getAverageFlow(const std::string& group, unsigned int gi) {
        if (_averageFlow.count(group) == 0) {
            return 0.0;
        }
        return _averageFlow[group][gi];
    }

    void calculateAverageFlow();

private:
    void calculateAverageFlowSingle();

};

#endif //RGS_GRIDBUFFER_H
