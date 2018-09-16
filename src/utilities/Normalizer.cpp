#include "Normalizer.h"

#include <iostream>

static const double BOLTZMANN_CONSTANT = 1.38e-23; // Boltzmann const // TODO: Make more precise

/*
	How we normalize variables:
	1) Incoming parameters are in C-system.
	2) In programm we use blank variables.
*/

void Normalizer::init() {
    m_dTemperature = 1500.0; // K // Maximum temperature in system
    m_dPressure = 150.0; // Pressure of Cs gas
    m_dDensity = m_dPressure / (BOLTZMANN_CONSTANT * m_dTemperature); // Just density
    m_dMass = 133 * 1.66e-27; // kg (Cs I guess)
    m_dVelocity = sqrt(BOLTZMANN_CONSTANT * m_dTemperature / m_dMass); // m / s (This is how we define V1)
    m_dLength = 6.07e-5; // calculated and used to understand size of the system
    m_dTime = m_dLength / m_dVelocity; // s (just timestep)
}

double Normalizer::normalize(const double& value, const Normalizer::Type& type) const {
    switch (type) {
        case Type::TEMPERATURE:
            return value / m_dTemperature;
        case Type::PRESSURE:
            return value / m_dPressure;
        case Type::DENSITY:
            return value / m_dDensity;
        case Type::FLOW:
            return value / (m_dDensity * m_dVelocity);
        case Type::LAMBDA:
            return value * m_dTime;
        case Type::LENGTH:
            return value / m_dLength;
        case Type::TIME:
            return value / m_dTime;
    }
}

void Normalizer::normalize(double& value, const Normalizer::Type& type) const {
    value = normalize(static_cast<const double&>(value), type);
}

double Normalizer::restore(const double& value, const Normalizer::Type& type) const {
    switch (type) {
        case Type::TEMPERATURE:
            return value * m_dTemperature;
        case Type::PRESSURE:
            return value * m_dPressure;
        case Type::DENSITY:
            return value * m_dDensity;
        case Type::FLOW:
            return value * (m_dDensity * m_dVelocity);
        case Type::LAMBDA:
            return value / m_dTime;
        case Type::LENGTH:
            return value * m_dLength;
        case Type::TIME:
            return value * m_dTime;
    }
}

void Normalizer::restore(double& value, const Normalizer::Type& type) const {
    value = restore(static_cast<const double&>(value), type);
}

std::ostream& operator<<(std::ostream& os, const Normalizer& normalizer) {
    os << "{";
    os << "Temp =  "  << normalizer.m_dTemperature << " K" << "; "
       << "Pres = "   << normalizer.m_dPressure    << " Pa" << "; "
       << "Den = "    << normalizer.m_dDensity     << " 1/m^3" << "; "
       << "Mass = "   << normalizer.m_dMass        << " Kg" << "; "
       << "Vel = "    << normalizer.m_dVelocity    << " m/s" << "; "
       << "Length = " << normalizer.m_dLength      << " m" << "; "
       << "Time = "   << normalizer.m_dTime        << " s";
    os << "}";
    return os;
}
