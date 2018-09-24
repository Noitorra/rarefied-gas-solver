#include "Normalizer.h"

#include <iostream>
#include <cmath>

static const double BOLTZMANN_CONSTANT = 1.38064852e-23; // Boltzmann const

/*
	How we normalize variables:
	1) Incoming parameters are in C-system.
	2) In programm we use blank variables.
*/

void Normalizer::init(double maxMass) {
    _temperature = 1500.0; // K // Maximum temperature in system
    _pressure = 150.0; // Pressure of Cs gas
    _density = _pressure / (BOLTZMANN_CONSTANT * _temperature); // Just density
    _velocity = std::sqrt(BOLTZMANN_CONSTANT * _temperature / _mass); // m / s (This is how we define V1)

    _length = 1.0; // time is calculated through system length, so no point in length normalization
    _time = _length / _velocity; // s (just timestep)

    _mass = maxMass;
}

double Normalizer::normalize(const double& value, const Normalizer::Type& type) const {
    switch (type) {
        case Type::TEMPERATURE:
            return value / _temperature;
        case Type::PRESSURE:
            return value / _pressure;
        case Type::DENSITY:
            return value / _density;
        case Type::FLOW:
            return value / (_density * _velocity);
        case Type::LAMBDA:
            return value * _time;
        case Type::LENGTH:
            return value / _length;
        case Type::TIME:
            return value / _time;
        case Type::MASS:
            return value / _mass;
    }
}

void Normalizer::normalize(double& value, const Normalizer::Type& type) const {
    value = normalize(static_cast<const double&>(value), type);
}

double Normalizer::restore(const double& value, const Normalizer::Type& type) const {
    switch (type) {
        case Type::TEMPERATURE:
            return value * _temperature;
        case Type::PRESSURE:
            return value * _pressure;
        case Type::DENSITY:
            return value * _density;
        case Type::FLOW:
            return value * (_density * _velocity);
        case Type::LAMBDA:
            return value / _time;
        case Type::LENGTH:
            return value * _length;
        case Type::TIME:
            return value * _time;
        case Type::MASS:
            return value * _mass;
    }
}

void Normalizer::restore(double& value, const Normalizer::Type& type) const {
    value = restore(static_cast<const double&>(value), type);
}

std::ostream& operator<<(std::ostream& os, const Normalizer& normalizer) {
    os << "{";
    os << "Temp =  "  << normalizer._temperature << " K" << "; "
       << "Pres = "   << normalizer._pressure    << " Pa" << "; "
       << "Den = "    << normalizer._density     << " 1/m^3" << "; "
       << "Mass = "   << normalizer._mass        << " Kg" << "; "
       << "Vel = "    << normalizer._velocity    << " m/s" << "; "
       << "Length = " << normalizer._length      << " m" << "; "
       << "Time = "   << normalizer._time        << " s";
    os << "}";
    return os;
}
