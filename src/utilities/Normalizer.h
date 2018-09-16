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
        TIME
    };

private:
    double m_dDensity; // density
    double m_dTemperature; // temperature
    double m_dPressure; // real normilize operation
    double m_dTime; // time
    double m_dMass; // mass
    double m_dVelocity; // speed cut
    double m_dLength; // the mean free path of a molecule

public:
    Normalizer() = default;

    void init();

    double normalize(const double& value, const Type& type) const;

    void normalize(double& value, const Type& type) const;

    double restore(const double& value, const Type& type) const;

    void restore(double& value, const Type& type) const;

    friend std::ostream& operator<<(std::ostream& os, const Normalizer& normalizer);

private:
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & m_dDensity;
        ar & m_dTemperature;
        ar & m_dPressure;
        ar & m_dTime;
        ar & m_dMass;
        ar & m_dVelocity;
        ar & m_dLength;
    }

};

#endif // NORMALIZER_H
