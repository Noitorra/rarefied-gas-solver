#include <utility>

#ifndef RGS_BORDERPARAMETERS_H
#define RGS_BORDERPARAMETERS_H

#include <string>
#include <vector>
#include <ostream>

#include <boost/serialization/access.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>

#include "utilities/Types.h"
#include "utilities/Utils.h"
#include "GradientParameter.h"

class BoundaryParameters {
    friend class boost::serialization::access;

private:
    std::string _group;
    std::vector<std::string> _type;
    std::vector<double> _temperature;
    std::vector<double> _pressure;
    std::vector<Vector3d> _flow;

    std::vector<GradientParameter> _gradientTemperature;

    std::vector<std::string> _connectGroups;

public:
    BoundaryParameters() = default;

    BoundaryParameters(std::string group,
                       std::vector<std::string> type,
                       std::vector<double> temperature,
                       std::vector<double> pressure,
                       std::vector<Vector3d> flow,
                       std::vector<GradientParameter> gradientTemperature,
                       std::vector<std::string> connectGroups) : _group(std::move(group)),
                                                                 _type(std::move(type)),
                                                                 _temperature(std::move(temperature)),
                                                                 _pressure(std::move(pressure)),
                                                                 _flow(std::move(flow)),
                                                                 _gradientTemperature(std::move(gradientTemperature)),
                                                                 _connectGroups(std::move(connectGroups)) {}

    const std::string& getGroup() const {
        return _group;
    }

    const std::vector<std::string>& getType() const {
        return _type;
    }

    double getTemperature(int gi) const {
        return _temperature[gi];
    }

    void setTemperature(int gi, double temperature) {
        _temperature[gi] = temperature;
    }

    double getPressure(int gi) const {
        return _pressure[gi];
    }

    void setPressure(int gi, double pressure) {
        _pressure[gi] = pressure;
    }

    const Vector3d& getFlow(int gi) const {
        return _flow[gi];
    }

    void setFlow(int gi, Vector3d flow) {
        _flow[gi] = std::move(flow);
    }

    const GradientParameter& getGradientTemperature(int gi) const {
        return _gradientTemperature[gi];
    }

    void setGradientTemperature(int gi, GradientParameter gradientTemperature) {
        _gradientTemperature[gi] = std::move(gradientTemperature);
    }

    bool hasGradientTemperature(int gi) const {
        return _gradientTemperature.size() > gi;
    }

    const std::string getConnectGroup(int gi) const {
        return _connectGroups[gi];
    }

    const std::vector<std::string>& getConnectGroups() const {
        return _connectGroups;
    }

    friend std::ostream& operator<<(std::ostream& os, const BoundaryParameters& parameters) {
        os << "{";
        os << "Group = " << parameters._group;
        if (parameters._type.empty() == false) {
            os << "; ";
            os << "Type = " << Utils::toString(parameters._type);
        }
        if (parameters._temperature.empty() == false) {
            os << "; ";
            os << "Temperature = " << Utils::toString(parameters._temperature);
        }
        if (parameters._pressure.empty() == false) {
            os << "; ";
            os << "Pressure = " << Utils::toString(parameters._pressure);
        }
        if (parameters._flow.empty() == false) {
            os << "; ";
            os << "Flow = " << Utils::toString(parameters._flow);
        }
        os << "}";
        return os;
    }

private:
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & _group;
        ar & _type;
        ar & _temperature;
        ar & _pressure;
        ar & _flow;
        ar & _gradientTemperature;
        ar & _connectGroups;
    }

};

#endif //RGS_BORDERPARAMETERS_H
