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
        HEATFLOW,
        LAMBDA,
        LENGTH,
        SQUARE,
        VOLUME,
        TIME,
        MASS,
        RADIUS
    };

private:
    double _density;
    double _temperature;
    double _pressure;
    double _time;
    double _mass;
    double _velocity;
    double _length;
    double _square;
    double _volume;
    double _radius;

public:
    Normalizer() = default;

    void init(double maxMass, double maxRadius, double maxPressure, double maxTemperature);

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
        ar & _square;
        ar & _volume;
        ar & _radius;
    }

};

#endif // NORMALIZER_H
