/*
 * impulse.cpp
 *
 *  Created on: 04 апр. 2014 г.
 *      Author: kisame
 */

#include "impulse.h"
#include <iostream>

Impulse::Impulse() {
	m_cSolverInfo = nullptr;
	m_dMaxImpulse = 4.8;
	m_uResolution = 20;

	m_dDeltaImpulse = 0.0;
	m_dDeltaImpulseQube = 0.0;
}

Impulse::~Impulse() {
	// TODO Auto-generated destructor stub
}

void Impulse::Init() {
	if( m_cSolverInfo == nullptr ) {
		std::cout << "[Error][Impulse] Before using Init, set SolverInfo..." << std::endl;
	} else {
		// calc delta impulse
		m_dDeltaImpulse = 2*m_dMaxImpulse/(m_uResolution - 1);
		m_dDeltaImpulseQube = std::pow(m_dDeltaImpulse, 3);

		// calc line impulse vector
		std::vector<double> line;
		for( unsigned int i=0; i<m_uResolution; i++ ) {
			line.push_back( m_dDeltaImpulse*i - m_dMaxImpulse );
			//std::cout << "[Impulse] impulse[" << i << "] = " << line.back() << std::endl;
		}

		// creating impulse sphere
		for(unsigned int x=0;x<line.size();x++) {
			for(unsigned int y=0;y<line.size();y++) {
				for(unsigned int z=0;z<line.size();z++) {
					Vector3d vec(line[x], line[y], line[z]);
					if( vec.mod() < m_dMaxImpulse ) {
						m_vImpulse.push_back( vec );
					}
				}
			}
		}


	}
}

void Impulse::setSolverInfo(SolverInfo* argSolverInfo) {
	m_cSolverInfo = argSolverInfo;
}

void Impulse::setMaxImpulse(double max_impulse) {
	m_dMaxImpulse = max_impulse;
}
void Impulse::setResolution(unsigned int resolution) {
	m_uResolution = resolution;
}

unsigned int Impulse::getResolution() {
	return m_uResolution;
}

double Impulse::getMaxImpulse() {
	return m_dMaxImpulse;
}

double Impulse::getDeltaImpulse() {
	return m_dDeltaImpulse;
}

double Impulse::getDeltaImpulseQube() {
	return m_dDeltaImpulseQube;
}

std::vector< Vector3d >& Impulse::getVector() {
	return m_vImpulse;
}
