#ifndef RGS_INITIALPARAMETERS_H
#define RGS_INITIALPARAMETERS_H

#include <string>
#include <vector>
#include <ostream>

#include <boost/serialization/access.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>

#include "utilities/Types.h"
#include "utilities/Utils.h"
#include "GradientParameter.h"

class InitialParameters {
    friend class boost::serialization::access;

private:
    std::string _group;
    std::vector<double> _pressure;
    std::vector<double> _temperature;

    std::vector<GradientParameter> _gradientTemperature;
    std::vector<GradientParameter> _gradientPressure;

public:
    InitialParameters() = default;

    InitialParameters(std::string group,
                      std::vector<double> pressure,
                      std::vector<double> temperature,
                      std::vector<GradientParameter> gradientTemperature,
                      std::vector<GradientParameter> gradientPressure) : _group(std::move(group)),
                                                                         _pressure(std::move(pressure)),
                                                                         _temperature(std::move(temperature)),
                                                                         _gradientTemperature(std::move(gradientTemperature)),
                                                                         _gradientPressure(std::move(gradientPressure)) {}

    const std::string& getGroup() const {
        return _group;
    }

    double getPressure(int gi) const {
        return _pressure[gi];
    }

    void setPressure(int gi, double pressure) {
        _pressure[gi] = pressure;
    }

    double getTemperature(int gi) const {
        return _temperature[gi];
    }

    void setTemperature(int gi, double temperature) {
        _temperature[gi] = temperature;
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

    const GradientParameter& getGradientPressure(int gi) const {
        return _gradientPressure[gi];
    }

    void setGradientPressure(int gi, GradientParameter gradientPressure) {
        _gradientPressure[gi] = std::move(gradientPressure);
    }

    bool hasGradientPressure(int gi) const {
        return _gradientPressure.size() > gi;
    }

    friend std::ostream& operator<<(std::ostream& os, const InitialParameters& parameters) {
        os << "{";
        os << "Group = " << parameters._group << "; "
           << "Pressure = " << Utils::toString(parameters._pressure) << "; "
           << "Temperature = " << Utils::toString(parameters._temperature);
        os << "}";
        return os;
    }

private:
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & _group;
        ar & _pressure;
        ar & _temperature;
        ar & _gradientTemperature;
    }

};

#endif //RGS_INITIALPARAMETERS_H
