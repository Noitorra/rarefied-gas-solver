#include "Normalizer.h"

#include <iostream>
#include <cmath>

static const double BOLTZMANN_CONSTANT = 1.38064852e-23; // Boltzmann const

/*
	How we normalize variables:
	1) Incoming parameters are in C-system.
	2) In programm we use blank variables.
*/

void Normalizer::init(double maxMass, double maxRadius, double maxPressure, double maxTemperature) {
    _mass = maxMass; // max mass
    _radius = maxRadius; // max radius of molecule

    if (std::abs(maxPressure) < 1e-16) {
        maxPressure = 1.0; // 1 Pa
    }

    _pressure = maxPressure;
    _temperature = maxTemperature;
    _density = maxPressure / (BOLTZMANN_CONSTANT * maxTemperature);
    _velocity = std::sqrt(BOLTZMANN_CONSTANT * maxTemperature / maxMass);

    _length = BOLTZMANN_CONSTANT * maxTemperature / (std::sqrt(2.0) * M_PI * maxPressure * std::pow(maxRadius * 2, 2)); // mean free path
    _square = _length * _length;
    _volume = _length * _length * _length;

    _time = _length / _velocity; // s (just timestep)
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
        case Type::HEATFLOW:
            return value / (_density * std::pow(_velocity, 3) * _mass);
        case Type::LAMBDA:
            return value * _time;
        case Type::LENGTH:
            return value / _length;
        case Type::SQUARE:
            return value / _square;
        case Type::VOLUME:
            return value / _volume;
        case Type::TIME:
            return value / _time;
        case Type::MASS:
            return value / _mass;
        case Type::RADIUS:
            return value / _radius;
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
        case Type::HEATFLOW:
            return value * (_density * std::pow(_velocity, 3) * _mass);
        case Type::LAMBDA:
            return value / _time;
        case Type::LENGTH:
            return value * _length;
        case Type::SQUARE:
            return value * _square;
        case Type::VOLUME:
            return value * _volume;
        case Type::TIME:
            return value * _time;
        case Type::MASS:
            return value * _mass;
        case Type::RADIUS:
            return value * _radius;
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
       << "Radius = " << normalizer._radius    << " m" << "; "
       << "Vel = "    << normalizer._velocity    << " m/s" << "; "
       << "Length = " << normalizer._length      << " m" << "; "
       << "Time = "   << normalizer._time        << " s";
    os << "}";
    return os;
}
