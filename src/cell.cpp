#include "cell.h"

#include "grid.h"
#include "grid_manager.h"
#include "solver.h"
#include "gas.h"
#include "impulse.h"
#include "config.h"

#include <iostream>
#include <algorithm>

#include "integral/ci.hpp"

Cell::Cell() :
m_pGridManager(nullptr),
m_pSolver(nullptr),
m_pGrid(nullptr)
{
	// TODO Auto-generated constructor stub
	m_vType.resize(3, CT_UNDEFINED);

	// Create 3 std::vector< Cell* > for dimentions
	m_vNext.resize(3);
	m_vPrev.resize(3);

	// size of a cell
	// m_vAreastep.resize(3, 0.0);

	m_dStartTemperature = 0.0;
  m_dStartConcentration = 0.0;

	// Grid....
	m_pGridManager = nullptr;
}

Cell::~Cell() {
	// TODO Auto-generated destructor stub
}

/* public */

// main methods
void Cell::setParameters(double _Concentration, double _Temperature, Vector3d _Areastep) {
  m_dStartConcentration = _Concentration;
	m_dStartTemperature = _Temperature;
	m_vAreastep = _Areastep;
}

void Cell::Init(GridManager* pGridManager) {
  m_pGridManager = pGridManager;
  m_pSolver = pGridManager->GetSolver();
  m_pGrid = pGridManager->GetGrid();
  
	GasVector& gasv = m_pSolver->GetGas();
	Impulse* impulse = m_pSolver->GetImpulse();
	ImpulseVector& impulsev = impulse->getVector();

  for(unsigned int gi=0;gi<gasv.size();gi++) {
    // Allocating space for values and half's
    m_vHalf.resize(gasv.size());
    m_vValue.resize(gasv.size());
    m_vMacroData.resize(gasv.size());

    m_vHalf[gi].resize( impulsev.size(), 0.0 );
    m_vValue[gi].resize( impulsev.size(), 0.0 );

    ResetSpeed(gi, m_dStartConcentration, m_dStartTemperature);
  }
}

// TODO: Change function name =)
void Cell::ResetSpeed(unsigned int gi, double dConcentration, double dTemperature) {
	GasVector& gasv = m_pSolver->GetGas();
	Impulse* impulse = m_pSolver->GetImpulse();
	ImpulseVector& impulsev = impulse->getVector();
  

  double C = 0.0;
  for(unsigned int ii=0;ii<impulsev.size();ii++) {
    C += fast_exp(gasv[gi]->getMass(), dTemperature, impulsev[ii]);
  }
  
  C *= impulse->getDeltaImpulseQube();
  C = dConcentration / C;
  
  for(unsigned int ii=0;ii<impulsev.size();ii++) {
    m_vValue[gi][ii] = C*fast_exp(gasv[gi]->getMass(), dTemperature, impulsev[ii]);
    //m_half[gasIndex][impulseIndex] = 0.0;
  }
}

void Cell::computeType(unsigned int dim) {
	compute_type(dim);
}

void Cell::computeHalf(unsigned int dim) {
	//std::cout << "Type: " << m_vType[dim] << std::endl;
	switch(m_vType[dim]) {
		case CT_UNDEFINED:
		//std::cout << "Cell::computeHalf() CT_UNDEFINED" << std::endl;
		break;
		case CT_LEFT:
		compute_half_left(dim);
		break;
		case CT_NORMAL:
		compute_half_normal(dim);
		break;
		case CT_PRERIGHT:
		compute_half_preright(dim);
		break;
		case CT_RIGHT:
		compute_half_right(dim);
		break;
		default:
		break;
	}
}
void Cell::computeValue(unsigned int dim) {
	switch(m_vType[dim]) {
		case CT_UNDEFINED:
		//std::cout << "Cell::computeValue() CT_UNDEFINED" << std::endl;
		break;
//		case CT_LEFT:
//		computeValue_Left(dim);
//		break;
		case CT_NORMAL:
		compute_value_normal(dim);
		break;
		case CT_PRERIGHT:
    compute_value_normal(dim);
		break;
//		case CT_RIGHT:
//		computeValue_Right(dim);
//		break;
		default:
		break;
	}
}
void Cell::computeIntegral(unsigned int gi0, unsigned int gi1) {
  ci::iter(m_vValue[gi0], m_vValue[gi1]);
}

void Cell::computeMacroData() {
	GasVector& gasv = m_pSolver->GetGas();

	for(unsigned int gi=0;gi<gasv.size();gi++) {
		m_vMacroData[gi].Concentration = compute_concentration(gi);
		m_vMacroData[gi].Stream = compute_stream(gi);
    m_vMacroData[gi].Temperature = compute_temperature(gi);
		m_vMacroData[gi].HeatStream = compute_heatstream(gi);
	}
}

/* Tests */
bool Cell::testInnerValuesRange() {
  GasVector& gasv = m_pSolver->GetGas();
  ImpulseVector& impulsev = m_pSolver->GetImpulse()->getVector();

  bool result = true;

  for(unsigned int gi=0;gi<gasv.size();gi++) {
    for(unsigned int ii=0;ii<impulsev.size();ii++) {
        if( 0.0 <= m_vValue[gi][ii] && m_vValue[gi][ii] <= 1.0 ) {
            // all is good
        } else {
            // wrong values
            result = false;
            std::cout << "Cell::testIVR(): Error [type][gi][ii]"
              << "[" << m_vType[0] << ":" << m_vType[1] << ":" << m_vType[2] << "]"
              << "[" << gi << "]"
              << "[" << ii << "]"
              << " value = " << m_vValue[gi][ii] << std::endl;
                }
                if( 0.0 <= m_vHalf[gi][ii] && m_vHalf[gi][ii] <= 1.0 ) {
                    // all is good
                } else {
                    // wrong values
                    result = false;
            std::cout << "Cell::testIVR(): Error [type][gi][ii]"
              << "[" << m_vType[0] << ":" << m_vType[1] << ":" << m_vType[2] << "]"
              << "[" << gi << "]"
              << "[" << ii << "]"
              << " half = " << m_vHalf[gi][ii] << std::endl;
        }
    }
  }

  return result;
}

/* private */

// help methods
void Cell::compute_type(unsigned int dim) {
	if( m_vPrev[dim].empty() ) {
		if ( !m_vNext[dim].empty() ) {
			m_vType[dim] = CT_LEFT;
		} else {
			//std::cout << "Cell has no neighbors in dim = " << dim << std::endl;
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
		//std::cout << "Cannot find this cell type, maybe cells are not linked, or linked wrong." << std::endl;
	}
}

void Cell::compute_half_left(unsigned int dim) {
	GasVector& gasv = m_pSolver->GetGas();
	ImpulseVector& impulsev = m_pSolver->GetImpulse()->getVector();

  for(unsigned int gi=0;gi<gasv.size();gi++) {
    double C1_up = 0.0;
    double C1_down = 0.0;
    double C2_up = 0.0;
    for(unsigned int ii=0;ii<impulsev.size();ii++) {
      if(impulsev[ii][dim] < 0) {
        double y = Config::dTimestep/gasv[gi]->getMass()*std::abs(impulsev[ii][dim]/m_vAreastep[dim]);

        m_vValue[gi][ii] = 2*compute_av(dim, gi, ii, AD_NEXT) - compute_av(dim, gi, ii, AD_NEXTNEXT);
        if (m_vValue[gi][ii] < 0) m_vValue[gi][ii] = 0;

        m_vHalf[gi][ii] = compute_av(dim, gi, ii, AD_NEXT) - (1 - y) / 2 * limiter(
         		m_vValue[gi][ii],
            compute_av(dim, gi, ii, AD_NEXT),
            compute_av(dim, gi, ii, AD_NEXTNEXT));

        C1_up += std::abs(impulsev[ii][dim]*m_vHalf[gi][ii]);
        C2_up += std::abs(impulsev[ii][dim] * (m_vValue[gi][ii] + compute_av(dim, gi, ii, AD_NEXT)) / 2);
      } else {
        C1_down += std::abs(impulsev[ii][dim] * fast_exp(gasv[gi]->getMass(), m_dStartTemperature, impulsev[ii]));
      }
    }

    for(unsigned int ii=0;ii<impulsev.size();ii++) {
      if(impulsev[ii][dim] > 0) {
        m_vHalf[gi][ii] = C1_up/C1_down*fast_exp(gasv[gi]->getMass(), m_dStartTemperature, impulsev[ii]);
        m_vValue[gi][ii] = 2 * C2_up / C1_down*fast_exp(gasv[gi]->getMass(), m_dStartTemperature, impulsev[ii]) - compute_av(dim, gi, ii, AD_NEXT);
        if(m_vValue[gi][ii] < 0.0) m_vValue[gi][ii] = 0.0;
      }
    }
  }
}
void Cell::compute_half_normal(unsigned int dim) {
	GasVector& gasv = m_pSolver->GetGas();
	ImpulseVector& impulsev = m_pSolver->GetImpulse()->getVector();

	for(unsigned int gi=0;gi<gasv.size();gi++) {
		for(unsigned int ii=0;ii<impulsev.size();ii++) {
      double y = Config::dTimestep / gasv[gi]->getMass()*std::abs(impulsev[ii][dim] / m_vAreastep[dim]);
      if (impulsev[ii][dim] > 0) {
        m_vHalf[gi][ii] = m_vValue[gi][ii] + (1 - y) / 2 * limiter(
          compute_av(dim, gi, ii, AD_PREV),
          m_vValue[gi][ii],
          compute_av(dim, gi, ii, AD_NEXT));
      }
      else {
        m_vHalf[gi][ii] = compute_av(dim, gi, ii, AD_NEXT) - (1 - y) / 2 * limiter(
          m_vValue[gi][ii],
          compute_av(dim, gi, ii, AD_NEXT),
          compute_av(dim, gi, ii, AD_NEXTNEXT));
      }
		}
	}
}
void Cell::compute_half_preright(unsigned int dim) {

}
void Cell::compute_half_right(unsigned int dim) {
	GasVector& gasv = m_pSolver->GetGas();
	ImpulseVector& impulsev = m_pSolver->GetImpulse()->getVector();

	for(unsigned int gi=0;gi<gasv.size();gi++) {
	    double C1_up = 0.0;
	    double C1_down = 0.0;
	    double C2_up = 0.0;
	    for(unsigned int ii=0;ii<impulsev.size();ii++) {
	      if(impulsev[ii][dim] > 0) {
	      	double y = Config::dTimestep/gasv[gi]->getMass()*std::abs(impulsev[ii][dim]/m_vAreastep[dim]);

          m_vValue[gi][ii] = 2 * compute_av(dim, gi, ii, AD_PREV) - compute_av(dim, gi, ii, AD_PREVPREV);
	        if (m_vValue[gi][ii] < 0) m_vValue[gi][ii] = 0;

	        // TODO: check
	        for( auto& item : m_vPrev[dim] ) {
            item->m_vHalf[gi][ii] = item->m_vValue[gi][ii] + (1 - y) / 2 * limiter(
              item->compute_av(dim, gi, ii, AD_PREV),
              item->m_vValue[gi][ii],
              item->compute_av(dim, gi, ii, AD_NEXT));
	        }
	        // old code
//	      	m_vPrev[dim]->m_vHalf[gi][ii] = m_vPrev[dim]->m_vValue[gi][ii] + (1-y)/2*limitter::super_bee(m_vPrev[dim]->m_vPrev[dim]->m_vValue[gi][ii],
//	                                                                                                               m_vPrev[dim]->m_vValue[gi][ii],
//	                                                                                                               m_vValue[gi][ii]);
          C1_up += std::abs(impulsev[ii][dim] * compute_ah(dim, gi, ii, AD_PREV)); 
          C2_up += std::abs(impulsev[ii][dim] * (m_vValue[gi][ii] + compute_av(dim, gi, ii, AD_PREV)) / 2);
	      } else {
          C1_down += std::abs(impulsev[ii][dim] * fast_exp(gasv[gi]->getMass(), m_dStartTemperature, impulsev[ii]));
	      }
	    }

	    for(unsigned int ii=0;ii<impulsev.size();ii++) {
	      if(impulsev[ii][dim] < 0) {
	      	// TODO: check
	      	for( auto& item : m_vPrev[dim] ) {
	      		item->m_vHalf[gi][ii] = C1_up/C1_down*fast_exp(gasv[gi]->getMass(), m_dStartTemperature, impulsev[ii]);
	      	}
//	      	m_vPrev[dim]->m_vHalf[gi][ii] = C1_up/C1_down*fast_exp(gasv[gi]->getMass(), T, impulsev[ii]);
          m_vValue[gi][ii] = 2 * C2_up / C1_down*fast_exp(gasv[gi]->getMass(), m_dStartTemperature, impulsev[ii]) - compute_av(dim, gi, ii, AD_PREV);
					if(m_vValue[gi][ii] < 0.0) m_vValue[gi][ii] = 0.0;
	      }
	    }
	  }
}

void Cell::compute_value_normal(unsigned int dim) {
	GasVector& gasv = m_pSolver->GetGas();
	ImpulseVector& impulsev = m_pSolver->GetImpulse()->getVector();

  for(unsigned int gi=0;gi<gasv.size();gi++) {
    for(unsigned int ii=0;ii<impulsev.size();ii++) {
      double y = Config::dTimestep / gasv[gi]->getMass()*impulsev[ii][dim] / m_vAreastep[dim];
        // compute_ah(dim, gi, ii, AD_PREV)
        m_vValue[gi][ii] = m_vValue[gi][ii] - y*(m_vHalf[gi][ii] - compute_ah(dim, gi, ii, AD_PREV));
        //if (m_vValue[gi][ii] < 0) {
        //  std::cout << m_vValue[gi][ii] << ":" << ii << std::endl;
        //}
    }
  }
}

// HELP METHODS
double Cell::compute_av(unsigned int dim, unsigned int gi, unsigned int ii, AverageDepth eAverageDepth) {
  double result = 0.0;
  unsigned int count = 0;
  switch (eAverageDepth) {
  case AD_NEXT:
    for (auto& item : m_vNext[dim]) {
      result += item->m_vValue[gi][ii];
      count++;
    }
    break;
  case AD_NEXTNEXT:
    for (auto& item1 : m_vNext[dim]) {
      for (auto& item2 : item1->m_vNext[dim]) {
        result += item2->m_vValue[gi][ii];
        count++;
      }
    }
    break;
  case AD_PREV:
    for (auto& item : m_vPrev[dim]) {
      result += item->m_vValue[gi][ii];
      count++;
    }
    break;
  case AD_PREVPREV:
    for (auto& item1 : m_vPrev[dim]) {
      for (auto& item2 : item1->m_vPrev[dim]) {
        result += item2->m_vValue[gi][ii];
        count++;
      }
    }
    break;
  }
  //if (count == 2) {
  //  std::cout << "Value" << eAverageDepth << " : " << count << " Type = " << m_vType[dim] << " result = " << result << std::endl;
  //  std::cout << ":" << m_vNext[dim].size() << ":" << m_vPrev[dim].size() << std::endl;
  //}

  if (count == 0 || count > 4)
    std::cout << "Value" << eAverageDepth << " : " << count << " Type = " << m_vType[dim] << std::endl;
  result /= count;
  return result;
}

double Cell::compute_ah(unsigned int dim, unsigned int gi, unsigned int ii, AverageDepth eAverageDepth) {
  double result = 0.0;
  unsigned int count = 0;
  switch (eAverageDepth) {
  case AD_NEXT:
    for (auto& item : m_vNext[dim]) {
      result += item->m_vHalf[gi][ii];
      count++;
    }
    break;
  case AD_NEXTNEXT:
    for (auto& item1 : m_vNext[dim]) {
      //result += item1->m_vHalf[gi][ii];
      //count++;
      for (auto& item2 : item1->m_vNext[dim]) {
        result += item2->m_vHalf[gi][ii];
        count++;
      }
    }
    break;
  case AD_PREV:
    for (auto& item : m_vPrev[dim]) {
      result += item->m_vHalf[gi][ii];
      count++;
    }
    break;
  case AD_PREVPREV:
    for (auto& item1 : m_vPrev[dim]) {
      for (auto& item2 : item1->m_vPrev[dim]) {
        result += item2->m_vHalf[gi][ii];
        count++;
      }
    }
    break;
  }
  if (count == 0 || count > 4)
    std::cout << "Half" << eAverageDepth << " : " << count << " Type = " << m_vType[dim] << std::endl;
  result /= count;
  return result;
}

double Cell::fast_exp(const double& mass, const double& temp, const Vector3d& impulse) {
	return std::exp(-impulse.mod2()/mass/2/temp);
}

double Cell::limiter_superbee(const double& x, const double& y, const double& z) {
  if( (z-y)*(y-x) <= 0 ) return 0.0;
  else return std::max(0.0, std::min(2*std::abs(y-x), std::min(std::abs(z-y), std::min(std::abs(y-x), 2*std::abs(z-y)))))*sgn(z-y);
}

/* Macro Data */

double Cell::compute_concentration(unsigned int gi) {
	Impulse* impulse = m_pSolver->GetImpulse();
	ImpulseVector& impulsev = impulse->getVector();

  double concentration = 0.0;
  for(unsigned int ii=0;ii<impulsev.size();ii++) {
  	concentration += m_vValue[gi][ii];
  }
  concentration *= impulse->getDeltaImpulseQube();
  return concentration;
}

double Cell::compute_temperature(unsigned int gi) {
	GasVector& gasv = m_pSolver->GetGas();
	Impulse* impulse = m_pSolver->GetImpulse();
	ImpulseVector& impulsev = impulse->getVector();

	double concentration = m_vMacroData[gi].Concentration;
  double temperature = 0.0;
  Vector3d vAverageSpeed = m_vMacroData[gi].Stream;
  vAverageSpeed /= concentration;

  for(unsigned int ii=0;ii<impulsev.size();ii++) {
    Vector3d tempvec;
    for (unsigned int vi = 0; vi < vAverageSpeed.getMass().size(); vi++) {
      tempvec[vi] = impulsev[ii][vi] / gasv[gi]->getMass() - vAverageSpeed[vi];
    }
    temperature += gasv[gi]->getMass()*tempvec.mod2()*m_vValue[gi][ii];
  }
  //cout << uvec[0] << ":" << uvec[1] << ":" << uvec[2] << endl;
  temperature *= impulse->getDeltaImpulseQube() / concentration / 3;

  //std::cout << temperature << std::endl;
  return temperature;
}

Vector3d Cell::compute_stream(unsigned int gi) {
	GasVector& gasv = m_pSolver->GetGas();
	Impulse* impulse = m_pSolver->GetImpulse();
  ImpulseVector& impulsev = impulse->getVector();

  Vector3d vStream;
  for (unsigned int ii = 0; ii<impulsev.size(); ii++) {
    for (unsigned int vi = 0; vi < vStream.getMass().size(); vi++) {
      vStream[vi] += impulsev[ii][vi] / gasv[gi]->getMass() * m_vValue[gi][ii];
    }
  }
  vStream *= impulse->getDeltaImpulseQube();

  return vStream;
}

Vector3d Cell::compute_heatstream(unsigned int gi) {
	return Vector3d();
}
