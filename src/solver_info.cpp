/*
 * solver_info.cpp
 *
 *  Created on: 04 апр. 2014 г.
 *      Author: kisame
 */

#include "solver_info.h"

#include "impulse.h"
#include "options.h"
#include "gas.h"

SolverInfo::SolverInfo()
: m_pImpulse(new Impulse),
  m_pOptions(new Options),
  m_pSolver(nullptr)
{
	// TODO Auto-generated constructor stub
	m_pImpulse->setSolverInfo( this );
	m_pOptions->setSolverInfo( this );

	// some variables
	m_vGas.push_back( std::shared_ptr<Gas>(new Gas(1.0)) );
	m_vGas.push_back( std::shared_ptr<Gas>(new Gas(0.5)) );
}

SolverInfo::~SolverInfo() {
	// TODO Auto-generated destructor stub
}

void SolverInfo::Init() {
	m_pImpulse->Init();
	m_pOptions->Init();
}

Config* SolverInfo::GetConfig() const {
  return m_pOptions->GetConfig();
}