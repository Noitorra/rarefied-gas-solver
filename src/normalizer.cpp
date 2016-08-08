#include "normalizer.h"
#include "config.h"

/*
	How we normalize variables:
	1) Incoming parameters are in C-system.
	2) In programm we use blank variables.
*/

Normalizer::Normalizer(): m_dDensity(0), m_dTemperature(0), m_dPressure(0), m_dTime(0), m_dMass(0), m_dVelocity(0), m_dLength(0) {}

void Normalizer::loadFromFile() {

    if (Config::getInstance()->isGPRTGrid()) {

        // From GPRT
        m_dTemperature = 600.0; // K (Lowest temperature?)
        m_dDensity = 1.81e22; // 1 / m^3
        m_dPressure = m_dDensity * sep::BOLTZMANN_CONSTANT * m_dTemperature;
        m_dMass = 133 * 1.66e-27; // kg
        m_dVelocity = sqrt(sep::BOLTZMANN_CONSTANT * m_dTemperature / m_dMass); // m / s
        m_dLength = 0.5 * 6e-4;
        m_dTime = m_dLength / m_dVelocity;
    } else {

        // From GRID
        m_dTemperature = 1500.0; // K // Maximum temperature in system
        m_dPressure = 150.0; // Pressure of Cs gas
        m_dDensity = m_dPressure / (sep::BOLTZMANN_CONSTANT * m_dTemperature); // Just density
        m_dMass = 133 * 1.66e-27; // kg (Cs I guess)
        m_dVelocity = sqrt(sep::BOLTZMANN_CONSTANT * m_dTemperature / m_dMass); // m / s (This is how we define V1)
        m_dLength = 6.07e-5; // calculated and used to understand size of the system
        m_dTime = m_dLength / m_dVelocity; // s (just timestep)
    }

}

double Normalizer::normalize(const double &value, const Normalizer::Type &type) const {
    switch(type) {
        case Type::TEMPERATURE:
            return value / m_dTemperature;
        case Type::PRESSURE:
            return value / m_dPressure;
        case Type::FLOW:
            return value / (m_dDensity * m_dVelocity);
        case Type::LAMBDA:
            return value * m_dTime;
    }
}

void Normalizer::normalize(double &value, const Normalizer::Type &type) const {
    value = normalize(static_cast<const double&>(value), type);
}
