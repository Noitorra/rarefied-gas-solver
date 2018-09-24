#ifndef NORMALIZER_H
#define NORMALIZER_H

#include <ostream>

#include <boost/serialization/access.hpp>
#include <boost/serialization/vector.hpp>

class Normalizer {
    friend class boost::serialization::access;

public:
    enum class Type {
        TEMPERATURE,
        PRESSURE,
        DENSITY,
        FLOW,
        LAMBDA,
        LENGTH,
        TIME,
        MASS
    };

private:
    double _density;
    double _temperature;
    double _pressure;
    double _time;
    double _mass;
    double _velocity;
    double _length;

public:
    Normalizer() = default;

    void init(double maxMass);

    double normalize(const double& value, const Type& type) const;

    void normalize(double& value, const Type& type) const;

    double restore(const double& value, const Type& type) const;

    void restore(double& value, const Type& type) const;

    friend std::ostream& operator<<(std::ostream& os, const Normalizer& normalizer);

private:
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & _density;
        ar & _temperature;
        ar & _pressure;
        ar & _time;
        ar & _mass;
        ar & _velocity;
        ar & _length;
    }

};

#endif // NORMALIZER_H
