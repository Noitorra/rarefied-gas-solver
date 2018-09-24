#ifndef RGS_INITIALPARAMETERS_H
#define RGS_INITIALPARAMETERS_H

#include "utilities/Utils.h"

#include <string>
#include <vector>
#include <ostream>

#include <boost/serialization/access.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>

class InitialParameters {
    friend class boost::serialization::access;

private:
    std::string _group;
    std::vector<double> _pressure;
    std::vector<double> _temperature;

public:
    InitialParameters() = default;

    InitialParameters(std::string group, std::vector<double> pressure, std::vector<double> temperature)
    : _group(std::move(group)), _pressure(std::move(pressure)), _temperature(std::move(temperature)) {}

    const std::string& getGroup() const {
        return _group;
    }

    const std::vector<double>& getPressure() const {
        return _pressure;
    }

    double getPressure(int gi) const {
        return _pressure[gi];
    }

    void setPressure(int gi, double pressure) {
        _pressure[gi] = pressure;
    }

    const std::vector<double>& getTemperature() const {
        return _temperature;
    }

    double getTemperature(int gi) const {
        return _temperature[gi];
    }

    void setTemperature(int gi, double temperature) {
        _temperature[gi] = temperature;
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
    }

};

#endif //RGS_INITIALPARAMETERS_H
