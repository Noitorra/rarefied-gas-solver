/*
 * cell.cpp
 *
 *  Created on: 04 апр. 2014 г.
 *      Author: kisame
 */

#include "cell.h"

#include "grid.h"
#include "solver.h"
#include "solver_info.h"
#include "gas.h"
#include "impulse.h"

#include <iostream>

Cell::Cell() {
	// TODO Auto-generated constructor stub
	m_vType.resize(3, CT_UNDEFINED);

	// Create 3 std::vector< Cell* > for dimentions
	m_vNext.resize(3);
	m_vPrev.resize(3);

	// size of a cell
	m_vAreastep.resize(3, 0.0);

	// Grid....
	m_pGrid = nullptr;
}

Cell::Cell(Grid* _Grid) : Cell() {
	setGrid( _Grid );
}

Cell::~Cell() {
	// TODO Auto-generated destructor stub
}

/* public */

// main methods
void Cell::setParameters(const double& _Density, const double& _Temperature, const DoubleVector& _Areastep) {
	m_dStartDensity = _Density;
	m_dStartTemperature = _Temperature;
	m_vAreastep = _Areastep;
}

void Cell::Init() {
	SolverInfo* sinfo = m_pGrid->getSolver()->getSolverInfo();
	GasVector& gasv = sinfo->getGasVector();
	Impulse* impulse = sinfo->getImpulse();
	ImpulseVector& impulsev = impulse->getVector();

  for(unsigned int gi=0;gi<gasv.size();gi++) {
    double C = 0.0;
    for(unsigned int ii=0;ii<impulsev.size();ii++) {
        C += fast_exp(gasv[gi]->getMass(), m_dStartTemperature, impulsev[ii]);
    }

    C *= impulse->getDeltaImpulseQube();
    C = m_dStartDensity/C;

    // Allocating space for values and half's
    m_vHalf[gi].resize( impulsev.size() );
    m_vValue[gi].resize( impulsev.size() );

    for(unsigned int ii=0;ii<impulsev.size();ii++) {
      m_vValue[gi][ii] = C*fast_exp(gasv[gi]->getMass(), m_dStartTemperature, impulsev[ii]);
      //m_half[gasIndex][impulseIndex] = 0.0;
    }
  }
}

void Cell::computeHalf(unsigned int dim) {
	switch(m_vType[dim]) {
		case CT_UNDEFINED:
		computeType(dim);
		computeHalf(dim);
		break;
		case CT_LEFT:
		computeHalf_Left(dim);
		break;
		case CT_NORMAL:
		computeHalf_Normal(dim);
		break;
		case CT_PRERIGHT:
		computeHalf_PreRight(dim);
		break;
		case CT_RIGHT:
		computeHalf_Right(dim);
		break;
		default:
		break;
	}
}
void Cell::computeValue(unsigned int dim) {
	switch(m_vType[dim]) {
		case CT_UNDEFINED:
		computeType(dim);
		computeValue(dim);
		break;
//		case CT_LEFT:
//		computeValue_Left(dim);
//		break;
		case CT_NORMAL:
		computeValue_Normal(dim);
		break;
//		case CT_PRERIGHT:
//		computeValue_PreRight(dim);
//		break;
//		case CT_RIGHT:
//		computeValue_Right(dim);
//		break;
		default:
		break;
	}
}
void Cell::computeIntegral(unsigned int dim) {
	// TODO:: Add realization
}

/* private */

// help methods
void Cell::computeType(unsigned int dim) {
	if( m_vPrev[dim].empty() ) {
		if ( !m_vNext[dim].empty() ) {
			m_vType[dim] = CT_LEFT;
		} else {
			std::cout << "Cell has no neighbors in dim = " << dim << std::endl;
		}
	} else {
		if( m_vNext[dim].empty() ) {
			m_vType[dim] = CT_RIGHT;
		} else {
			if( m_vNext[dim][0]->m_vNext[dim].empty() ) {
				m_vType[dim] = CT_PRERIGHT;
			} else {
				m_vType[dim] = CT_NORMAL;
			}
		}
	}

	if( m_vType[dim] == CT_UNDEFINED) {
		std::cout << "Cannot find this cell type, maybe cells are not linked, or linked wrong." << std::endl;
	}
}

void Cell::computeHalf_Left(unsigned int dim) {
	SolverInfo* sinfo = m_pGrid->getSolver()->getSolverInfo();
	GasVector& gasv = sinfo->getGasVector();
	ImpulseVector& impulsev = sinfo->getImpulse()->getVector();

  for(unsigned int gi=0;gi<gasv.size();gi++) {
    double C1_up = 0.0;
    double C1_down = 0.0;
    double C2_up = 0.0;
    for(unsigned int ii=0;ii<impulsev.size();ii++) {
      if(impulsev[ii][dim] < 0) {
        double y = sinfo->getTimestep()/gasv[gi]->getMass()*std::abs(impulsev[ii][dim]/m_vAreastep[dim]);

        m_vValue[gi][ii] = 2*compute_anv(m_vNext[dim], dim, gi, ii, 0) - compute_anv(m_vNext[dim], dim, gi, ii, 1);
        if (m_vValue[gi][ii] < 0) m_vValue[gi][ii] = 0;

        m_vHalf[gi][ii] = compute_anv(m_vNext[dim], dim, gi, ii, 0) - (1-y)/2*limiter(
         		m_vValue[gi][ii],
         		compute_anv(m_vNext[dim], dim, gi, ii, 0),
         		compute_anv(m_vNext[dim], dim, gi, ii, 1) );

        C1_up += abs(impulsev[ii][dim]*m_vHalf[gi][ii]);
        C2_up += abs(impulsev[ii][dim]*(m_vValue[gi][ii] + compute_anv(m_vNext[dim], dim, gi, ii, 0))/2);
      } else {
        C1_down += abs(impulsev[ii][dim]*fast_exp(gasv[gi]->getMass(), m_dStartTemperature, impulsev[ii]));
      }
    }

    for(unsigned int ii=0;ii<impulsev.size();ii++) {
      if(impulsev[ii][dim] > 0) {
        m_vHalf[gi][ii] = C1_up/C1_down*fast_exp(gasv[gi]->getMass(), m_dStartTemperature, impulsev[ii]);
        m_vValue[gi][ii] = 2*C2_up/C1_down*fast_exp(gasv[gi]->getMass(), m_dStartTemperature, impulsev[ii]) - compute_anv(m_vNext[dim], dim, gi, ii, 0);
        if(m_vValue[gi][ii] < 0.0) m_vValue[gi][ii] = 0.0;
      }
    }
  }
}
void Cell::computeHalf_Normal(unsigned int dim) {
	SolverInfo* sinfo = m_pGrid->getSolver()->getSolverInfo();
	GasVector& gasv = sinfo->getGasVector();
	ImpulseVector& impulsev = sinfo->getImpulse()->getVector();

	for(unsigned int gi=0;gi<gasv.size();gi++) {
		for(unsigned int ii=0;ii<impulsev.size();ii++) {
			double y = sinfo->getTimestep()/gasv[gi]->getMass()*std::abs(impulsev[ii][dim]/m_vAreastep[dim]);
			if(impulsev[ii][dim] > 0) {
				m_vHalf[gi][ii] = m_vValue[gi][ii] + (1-y)/2*limiter(
						compute_apv(m_vPrev[dim], dim, gi, ii, 0),
						m_vValue[gi][ii],
						compute_anv(m_vNext[dim], dim, gi, ii, 0) );
			} else {
				m_vHalf[gi][ii] = compute_anv(m_vNext[dim], dim, gi, ii, 0) - (1-y)/2*limiter(
						m_vValue[gi][ii],
						compute_anv(m_vNext[dim], dim, gi, ii, 0),
						compute_anv(m_vNext[dim], dim, gi, ii, 1) );
			}
		}
	}
}
void Cell::computeHalf_PreRight(unsigned int dim) {

}
void Cell::computeHalf_Right(unsigned int dim) {
	SolverInfo* sinfo = m_pGrid->getSolver()->getSolverInfo();
	GasVector& gasv = sinfo->getGasVector();
	ImpulseVector& impulsev = sinfo->getImpulse()->getVector();

	for(unsigned int gi=0;gi<gasv.size();gi++) {
	    double C1_up = 0.0;
	    double C1_down = 0.0;
	    double C2_up = 0.0;
	    for(unsigned int ii=0;ii<impulsev.size();ii++) {
	      if(impulsev[ii][dim] > 0) {
	      	double y = sinfo->getTimestep()/gasv[gi]->getMass()*std::abs(impulsev[ii][dim]/m_vAreastep[dim]);

	         m_vValue[gi][ii] = 2*compute_apv(m_vPrev[dim], dim, gi, ii, 0) - compute_apv(m_vPrev[dim], dim, gi, ii, 1);
	        if (m_vValue[gi][ii] < 0) m_vValue[gi][ii] = 0;

	        // TODO: check
	        for( auto& item : m_vPrev[dim] ) {
	        	item->m_vHalf[gi][ii] = item->m_vValue[gi][ii] + (1-y)/2*limiter(
	        			compute_apv(item->m_vPrev[dim], dim, gi, ii, 0),
	         			item->m_vValue[gi][ii],
	         			compute_anv(item->m_vNext[dim], dim, gi, ii, 0) );
	        }
	        // old code
//	      	m_vPrev[dim]->m_vHalf[gi][ii] = m_vPrev[dim]->m_vValue[gi][ii] + (1-y)/2*limitter::super_bee(m_vPrev[dim]->m_vPrev[dim]->m_vValue[gi][ii],
//	                                                                                                               m_vPrev[dim]->m_vValue[gi][ii],
//	                                                                                                               m_vValue[gi][ii]);

	        C1_up += abs(impulsev[ii][dim]*compute_aph(m_vNext[dim], dim, gi, ii, 0));
	        C2_up += abs(impulsev[ii][dim]*(m_vValue[gi][ii] + compute_apv(m_vPrev[dim], dim, gi, ii, 0))/2);
	      } else {
	        C1_down += abs(impulsev[ii][dim]*fast_exp(gasv[gi]->getMass(), m_dStartTemperature, impulsev[ii]));
	      }
	    }

	    for(unsigned int ii=0;ii<impulsev.size();ii++) {
	      if(impulsev[ii][dim] < 0) {
	      	// TODO: check
	      	for( auto& item : m_vPrev[dim] ) {
	      		item->m_vHalf[gi][ii] = C1_up/C1_down*fast_exp(gasv[gi]->getMass(), m_dStartTemperature, impulsev[ii]);
	      	}
//	      	m_vPrev[dim]->m_vHalf[gi][ii] = C1_up/C1_down*fast_exp(gasv[gi]->getMass(), T, impulsev[ii]);
					m_vValue[gi][ii] = 2*C2_up/C1_down*fast_exp(gasv[gi]->getMass(), m_dStartTemperature, impulsev[ii]) - compute_apv(m_vPrev[dim], dim, gi, ii, 0);
					if(m_vValue[gi][ii] < 0.0) m_vValue[gi][ii] = 0.0;
	      }
	    }
	  }
}

void Cell::computeValue_Normal(unsigned int dim) {
	SolverInfo* sinfo = m_pGrid->getSolver()->getSolverInfo();
	GasVector& gasv = sinfo->getGasVector();
	ImpulseVector& impulsev = sinfo->getImpulse()->getVector();

  for(unsigned int gi=0;gi<gasv.size();gi++) {
    for(unsigned int ii=0;ii<impulsev.size();ii++) {
		    double y = sinfo->getTimestep()/gasv[gi]->getMass()*impulsev[ii][dim]/m_vAreastep[dim];
        m_vValue[gi][ii] = m_vValue[gi][ii] - y*(m_vHalf[gi][ii] - compute_aph(m_vPrev[dim], dim, gi, ii, 0));
    }
  }
}

// HELP METHODS
double Cell::compute_apv(CellVector& cellv, unsigned int dim, unsigned int gi, unsigned int ii, unsigned int depth ) {
	double result = 0.0;
	unsigned int count = 0;
	for( auto& item : cellv ) {
		result += item->m_vValue[gi][ii];
		count++;
		if( depth > 0) {
			result += compute_apv( item->m_vPrev[dim], dim, gi, ii, depth - 1 );
			count++;
		}
	}
	return result/count;
}

double Cell::compute_anv(CellVector& cellv, unsigned int dim, unsigned int gi, unsigned int ii, unsigned int depth ) {
	double result = 0.0;
	unsigned int count = 0;
	for( auto& item : cellv ) {
		result += item->m_vValue[gi][ii];
		count++;
		if( depth > 0) {
			result += compute_anv( item->m_vNext[dim], dim, gi, ii, depth - 1 );
			count++;
		}
	}
	return result/count;
}

double Cell::compute_aph(CellVector& cellv, unsigned int dim, unsigned int gi, unsigned int ii, unsigned int depth ) {
	double result = 0.0;
	unsigned int count = 0;
	for( auto& item : cellv ) {
		result += item->m_vHalf[gi][ii];
		count++;
		if( depth > 0) {
			result += compute_apv( item->m_vPrev[dim], dim, gi, ii, depth - 1 );
			count++;
		}
	}
	return result/count;
}

double Cell::compute_anh(CellVector& cellv, unsigned int dim, unsigned int gi, unsigned int ii, unsigned int depth ) {
	double result = 0.0;
	unsigned int count = 0;
	for( auto& item : cellv ) {
		result += item->m_vHalf[gi][ii];
		count++;
		if( depth > 0) {
			result += compute_anv( item->m_vNext[dim], dim, gi, ii, depth - 1 );
			count++;
		}
	}
	return result/count;
}

double Cell::fast_exp(const double& mass, const double& temp, const Vector3d& impulse) {
	return std::exp(-impulse.mod()/mass/2/temp);
}

double Cell::limiter_superbee(const double& x, const double& y, const double& z) {
  if( (z-y)*(y-x) <= 0 ) return 0.0;
  else return std::max(0.0, std::min(2*std::abs(y-x), std::min(std::abs(z-y), std::min(std::abs(y-x), 2*std::abs(z-y)))))*sgn(z-y);
}
