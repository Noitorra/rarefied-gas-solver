/*
 * options.cpp
 *
 *  Created on: 04 апр. 2014 г.
 *      Author: kisame
 */

#include "options.h"

Options::Options() {
	// TODO Auto-generated constructor stub
	m_cSolverInfo = nullptr;
}

Options::~Options() {
	// TODO Auto-generated destructor stub
}

void Options::setSolverInfo(SolverInfo* _SolverInfo) {
	m_cSolverInfo = _SolverInfo;
}

SolverInfo* Options::getSolverInfo() {
	return m_cSolverInfo;
}
