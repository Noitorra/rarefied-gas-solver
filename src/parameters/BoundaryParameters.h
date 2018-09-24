#ifndef RGS_BORDERPARAMETERS_H
#define RGS_BORDERPARAMETERS_H

#include "utilities/Utils.h"

#include <string>
#include <vector>
#include <ostream>

#include <boost/serialization/access.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>

class BoundaryParameters {
    friend class boost::serialization::access;

private:
    std::string _group;
    std::vector<std::string> _type;
    std::vector<double> _temperature;
    std::vector<double> _pressure;

public:
    BoundaryParameters() = default;

    BoundaryParameters(std::string group, std::vector<std::string> type, std::vector<double> temperature, std::vector<double> pressure)
    : _group(std::move(group)), _type(std::move(type)), _temperature(std::move(temperature)), _pressure(std::move(pressure)) {}

    const std::string& getGroup() const {
        return _group;
    }

    const std::vector<std::string>& getType() const {
        return _type;
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

    const std::vector<double>& getPressure() const {
        return _pressure;
    }

    double getPressure(int gi) const {
        return _pressure[gi];
    }

    void setPressure(int gi, double pressure) {
        _pressure[gi] = pressure;
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
    }

};

#endif //RGS_BORDERPARAMETERS_H
