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
    std::string _type;
    std::vector<double> _temperature;

public:
    BoundaryParameters() = default;

    BoundaryParameters(std::string group, std::string _type, std::vector<double> temperature)
    : _group(std::move(group)), _type(std::move(_type)), _temperature(std::move(temperature)) {}

    const std::string& getGroup() const {
        return _group;
    }

    const std::string& getType() const {
        return _type;
    }

    const std::vector<double>& getTemperature() const {
        return _temperature;
    }

    friend std::ostream& operator<<(std::ostream& os, const BoundaryParameters& parameters) {
        os << "{";
        os << "Group = " << parameters._group << "; "
           << "Type = " << parameters._type;
        if (parameters._temperature.empty() == false) {
            os << "; ";
            os << "Temperature = " << Utils::toString(parameters._temperature);
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
    }

};

#endif //RGS_BORDERPARAMETERS_H
