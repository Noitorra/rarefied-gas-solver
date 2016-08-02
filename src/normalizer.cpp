#include "normalizer.h"
#include <cmath>

const double BOLTZMANN_CONSTANT = 1.38e-23; // Bolzman const // TODO: Make more precise

/*
	How we normalize variables:
	1) Incoming parameters are in C-system.
	2) In programm we use blank variables.

	
*/

Normalizer::Normalizer(): n(0), t(0), p(0), tau(0), m(0), e(0), l(0) {}

void Normalizer::loadFromFile() {

	// From GPRT
	t = 600.0; // K (Lowest temperature?)
	n = 1.81e22; // 1 / m^3
	p = n * BOLTZMANN_CONSTANT * t;
	m = 133 * 1.66e-27; // kg
	e = sqrt(BOLTZMANN_CONSTANT * t / m); // m / s
	l = 0.5 * 6e-4;
	tau = l / e;

	// From GRID
	t = 1500.0; // K // Maximum temperature in system
	p = 150.0; // Pressure of Cs gas
	n = p / (BOLTZMANN_CONSTANT * t); // Just density
	m = 133 * 1.66e-27; // kg (Cs I guess)
	e = sqrt(BOLTZMANN_CONSTANT * t / m); // m / s (This is how we define V1)
	l = 6.07e-5; // calculated and used to understand size of the system
	tau = l / e; // s (just timestep)

}
