#include "cell.h"

#include "grid_manager.h"
#include "solver.h"
#include "parameters/gas.h"
#include "parameters/impulse.h"
#include "config.h"

#include "integral/ci.hpp"

#include <iostream>
#include <algorithm>

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

  m_vStartTemperature.resize(Config::iGasesNumber);
  m_vStartPressure.resize(Config::iGasesNumber);

  m_vBoundaryType.resize(Config::iGasesNumber);
  m_vBoundaryTemperature.resize(Config::iGasesNumber);
  m_vBoundaryStream.resize(Config::iGasesNumber);
  m_vBoundaryPressure.resize(Config::iGasesNumber);

	// Grid....
	m_pGridManager = nullptr;
}

Cell::~Cell() {
	// TODO Auto-generated destructor stub
}

/* public */

// main methods
void Cell::setParameters(double _Pressure, double _Temperature, Vector3d _Areastep, int _GasIndex) {
  m_vStartPressure[_GasIndex] = _Pressure;
	m_vStartTemperature[_GasIndex] = _Temperature;
	m_vAreastep = _Areastep;
}

void Cell::setBoundaryType(sep::BoundaryType eBoundaryType, double dTemperature, Vector3d dStream, double dPressure, int iGasIndex) {
  m_vBoundaryType[iGasIndex] = eBoundaryType;
  m_vBoundaryTemperature[iGasIndex] = dTemperature;
	m_vBoundaryStream[iGasIndex] = dStream;
  m_vBoundaryPressure[iGasIndex] = dPressure;
}

void Cell::Init(GridManager* pGridManager) {
  m_pGridManager = pGridManager;
  m_pSolver = pGridManager->GetSolver();
  m_pGrid = pGridManager->GetGrid();
  
  GasVector& gasv = Config::vGas;
	Impulse* impulse = m_pSolver->GetImpulse();
	ImpulseVector& impulsev = impulse->getVector();

  for(unsigned int gi=0;gi<Config::iGasesNumber;gi++) {
    // Allocating space for values and half's
    m_vHalf.resize(Config::iGasesNumber);
    m_vValue.resize(Config::iGasesNumber);
    m_vMacroData.resize(Config::iGasesNumber);

    m_vHalf[gi].resize( impulsev.size(), 0.0 );
    m_vValue[gi].resize( impulsev.size(), 0.0 );

    ResetSpeed(gi, m_vStartPressure[gi] / m_vStartTemperature[gi], m_vStartTemperature[gi]);
  }
}

// TODO: Change function name =)
void Cell::ResetSpeed(unsigned int gi, double dConcentration, double dTemperature) {
  GasVector& gasv = Config::vGas;
  Impulse* impulse = m_pSolver->GetImpulse();
  ImpulseVector& impulsev = impulse->getVector();

  double C = 0.0;
  for (unsigned int ii = 0; ii < impulsev.size(); ii++) {
    C += fast_exp(gasv[gi]->getMass(), dTemperature, impulsev[ii]);
  }

  C *= impulse->getDeltaImpulseQube();
  C = dConcentration / C;

  for (unsigned int ii = 0; ii < impulsev.size(); ii++) {
    m_vValue[gi][ii] = C*fast_exp(gasv[gi]->getMass(), dTemperature, impulsev[ii]);
    //m_half[gasIndex][impulseIndex] = 0.0;
  }
}

void Cell::computeType(unsigned int dim) {
	compute_type(dim);
}

void Cell::computeHalf(unsigned int dim) {
  //std::cout << "Type: " << m_vType[dim] << std::endl;
  switch (m_vType[dim]) {
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
  switch (m_vType[dim]) {
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

void Cell::computeBetaDecay(unsigned int gi0, unsigned int gi1, double lambda) {
	ImpulseVector& impulsev = m_pSolver->GetImpulse()->getVector();

	double dTempValue = 0.0;
	for (unsigned int ii = 0; ii < impulsev.size(); ii++) {
		dTempValue = m_vValue[gi0][ii] * lambda * Config::dTimestep; // TODO: check lambda * Config::dTimestep < 1 !!!
		m_vValue[gi0][ii] -= dTempValue;
		m_vValue[gi1][ii] += dTempValue;
	}
}

void Cell::computeMacroData() {
  GasVector& gasv = Config::vGas;

  if (!isValid()) {
    for (unsigned int gi = 0; gi < Config::iGasesNumber; gi++) {
      m_vMacroData[gi].C = 0.0;
      m_vMacroData[gi].Stream = Vector3d();
      m_vMacroData[gi].T = 0.0;
      m_vMacroData[gi].P = 0.0;
      m_vMacroData[gi].HeatStream = Vector3d();
    }
  } else {
    for (unsigned int gi = 0; gi < Config::iGasesNumber; gi++) {
      m_vMacroData[gi].C = compute_concentration(gi);
      m_vMacroData[gi].Stream = compute_stream(gi);
      m_vMacroData[gi].T = (m_vMacroData[gi].C == 0.0) ? 0.0 : compute_temperature(gi);
      m_vMacroData[gi].P = (m_vMacroData[gi].C == 0.0) ? 0.0 : compute_pressure(gi);
      m_vMacroData[gi].HeatStream = compute_heatstream(gi);
    }
  }
}

bool Cell::isValid()
{
  bool notX = m_vType[sep::X] == CT_LEFT || m_vType[sep::X] == CT_RIGHT;
  bool notY = m_vType[sep::Y] == CT_LEFT || m_vType[sep::Y] == CT_RIGHT;
  bool notZ = m_vType[sep::Z] == CT_LEFT || m_vType[sep::Z] == CT_RIGHT;
  return !notX && !notY && !notZ;
}

/* Tests */
bool Cell::testInnerValuesRange() {
  GasVector& gasv = Config::vGas;
  ImpulseVector& impulsev = m_pSolver->GetImpulse()->getVector();

  bool result = true;

  for (unsigned int gi = 0; gi < Config::iGasesNumber; gi++) {
    for (unsigned int ii = 0; ii < impulsev.size(); ii++) {
      if (m_vValue[gi][ii] < 0) {
        // wrong values
        result = false;
        std::cout << "Cell::testIVR(): Error [type][gi][ii]"
          << "[" << m_vType[0] << ":" << m_vType[1] << ":" << m_vType[2] << "]"
          << "[" << gi << "]"
          << "[" << ii << "]"
          << " value = " << m_vValue[gi][ii] << std::endl;
      }
      if (m_vHalf[gi][ii] < 0) {
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
  if (m_vPrev[dim].empty()) {
    if (!m_vNext[dim].empty()) {
      m_vType[dim] = CT_LEFT;
    }
    else {
      //std::cout << "Cell has no neighbors in dim = " << dim << std::endl;
    }
  }
  else {
    if (m_vNext[dim].empty()) {
      m_vType[dim] = CT_RIGHT;
    }
    else {
      if (m_vNext[dim][0]->m_vNext[dim].empty()) {
        m_vType[dim] = CT_PRERIGHT;
      }
      else {
        m_vType[dim] = CT_NORMAL;
      }
    }
  }

  if (m_vType[dim] == CT_UNDEFINED) {
    //std::cout << "Cannot find this cell type, maybe cells are not linked, or linked wrong." << std::endl;
  }
}

void Cell::compute_half_left(unsigned int dim) {
  for (int gi = 0; gi < Config::iGasesNumber; gi++) {
    switch (m_vBoundaryType[gi])
    {
    case sep::BT_DIFFUSE:
      compute_half_diffuse_left(dim, gi);
      break;
    case sep::BT_GASE:
			compute_half_gase_left(dim, gi);
      break;
    case sep::BT_MIRROR:
      compute_half_mirror_left(dim, gi);
      break;
		case sep::BT_FLOW:
			compute_half_flow_left(dim, gi);
			break;
    default:
      break;
    }
  }
}

void Cell::compute_half_normal(unsigned int dim) {
  GasVector& gasv = Config::vGas;
  Impulse* pImpulse = m_pSolver->GetImpulse();
  ImpulseVector& impulsev = pImpulse->getVector();

  for (unsigned int gi = 0; gi < Config::iGasesNumber; gi++) {
    for (unsigned int ii = 0; ii<impulsev.size(); ii++) {
      double y = Config::dTimestep / gasv[gi]->getMass()*std::abs(impulsev[ii][dim] / m_vAreastep[dim]);

      if (impulsev[ii][dim] > 0) {
        m_vHalf[gi][ii] = m_vValue[gi][ii] + (1 - y) / 2 * limiter(
          compute_av(dim, gi, ii, AD_PREV), // requires reverse MT_BEGIN
          m_vValue[gi][ii],
          compute_av(dim, gi, ii, AD_NEXT)); // requires reverse MT_END
      }
      else {
        m_vHalf[gi][ii] = compute_av(dim, gi, ii, AD_NEXT) - (1 - y) / 2 * limiter(
          m_vValue[gi][ii],
          compute_av(dim, gi, ii, AD_NEXT), // requires reverse MT_END
          compute_av(dim, gi, ii, AD_NEXTNEXT)); // requires reverse MT_END
      }
    }
  }
}

void Cell::compute_half_preright(unsigned int dim) {

}

void Cell::compute_half_right(unsigned int dim) {
  for (int gi = 0; gi < Config::iGasesNumber; gi++) {
    switch (m_vBoundaryType[gi])
    {
    case sep::BT_DIFFUSE:
      compute_half_diffuse_right(dim, gi);
      break;
		case sep::BT_GASE:
			compute_half_gase_right(dim, gi);
			break;
    case sep::BT_MIRROR:
      compute_half_mirror_right(dim, gi);
      break;
		case sep::BT_FLOW:
			compute_half_flow_right(dim, gi);
			break;
    default:
      break;
    }
  }
}

void Cell::compute_value_normal(unsigned int dim) {
  GasVector& gasv = Config::vGas;
  Impulse* pImpulse = m_pSolver->GetImpulse();
  ImpulseVector& impulsev = pImpulse->getVector();

  for (unsigned int gi = 0; gi < Config::iGasesNumber; gi++) {
    for (unsigned int ii = 0; ii < impulsev.size(); ii++) {
      double y = Config::dTimestep / gasv[gi]->getMass()*impulsev[ii][dim] / m_vAreastep[dim]; // requires reverse MT_BEGIN

      m_vValue[gi][ii] = m_vValue[gi][ii] - y*(m_vHalf[gi][ii] - compute_ah(dim, gi, ii, AD_PREV)); // requires reverse MT_BEGIN
    }
  }
}

/* Diffuse boundary type. */
void Cell::compute_half_diffuse_left(unsigned int dim, int gi)
{
  GasVector& gasv = Config::vGas;
  ImpulseVector& impulsev = m_pSolver->GetImpulse()->getVector();

  double C1_up = 0.0;
  double C1_down = 0.0;
  double C2_up = 0.0;
  for (unsigned int ii = 0; ii<impulsev.size(); ii++) {
    if (impulsev[ii][dim] < 0) {
      double y = Config::dTimestep / gasv[gi]->getMass()*std::abs(impulsev[ii][dim] / m_vAreastep[dim]);

      m_vValue[gi][ii] = 2 * compute_av(dim, gi, ii, AD_NEXT) - compute_av(dim, gi, ii, AD_NEXTNEXT);
      if (m_vValue[gi][ii] < 0) m_vValue[gi][ii] = 0;

      m_vHalf[gi][ii] = compute_av(dim, gi, ii, AD_NEXT) - (1 - y) / 2 * limiter(
        m_vValue[gi][ii],
        compute_av(dim, gi, ii, AD_NEXT),
        compute_av(dim, gi, ii, AD_NEXTNEXT));

      C1_up += std::abs(impulsev[ii][dim] * m_vHalf[gi][ii]);
      C2_up += std::abs(impulsev[ii][dim] * (m_vValue[gi][ii] + compute_av(dim, gi, ii, AD_NEXT)) / 2);
    }
    else {
      C1_down += std::abs(impulsev[ii][dim] * fast_exp(gasv[gi]->getMass(), m_vBoundaryTemperature[gi], impulsev[ii]));
    }
  }

  for (unsigned int ii = 0; ii<impulsev.size(); ii++) {
    if (impulsev[ii][dim] > 0) {
      m_vHalf[gi][ii] = C1_up / C1_down*fast_exp(gasv[gi]->getMass(), m_vBoundaryTemperature[gi], impulsev[ii]);
      m_vValue[gi][ii] = 2 * C2_up / C1_down*fast_exp(gasv[gi]->getMass(), m_vBoundaryTemperature[gi], impulsev[ii]) - compute_av(dim, gi, ii, AD_NEXT);
      if (m_vValue[gi][ii] < 0.0) m_vValue[gi][ii] = 0.0;
    }
  }
}

void Cell::compute_half_diffuse_right(unsigned int dim, int gi)
{
  GasVector& gasv = Config::vGas;
  ImpulseVector& impulsev = m_pSolver->GetImpulse()->getVector();

  double C1_up = 0.0;
  double C1_down = 0.0;
  double C2_up = 0.0;
  for (unsigned int ii = 0; ii<impulsev.size(); ii++) {
    if (impulsev[ii][dim] > 0) {
      double y = Config::dTimestep / gasv[gi]->getMass()*std::abs(impulsev[ii][dim] / m_vAreastep[dim]);

      m_vValue[gi][ii] = 2 * compute_av(dim, gi, ii, AD_PREV) - compute_av(dim, gi, ii, AD_PREVPREV);
      if (m_vValue[gi][ii] < 0) m_vValue[gi][ii] = 0;

      // TODO: check
      for (auto& item : m_vPrev[dim]) {
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
    }
    else {
      C1_down += std::abs(impulsev[ii][dim] * fast_exp(gasv[gi]->getMass(), m_vBoundaryTemperature[gi], impulsev[ii]));
    }
  }

  for (unsigned int ii = 0; ii<impulsev.size(); ii++) {
    if (impulsev[ii][dim] < 0) {
      // TODO: check
      for (auto& item : m_vPrev[dim]) {
        item->m_vHalf[gi][ii] = C1_up / C1_down*fast_exp(gasv[gi]->getMass(), m_vBoundaryTemperature[gi], impulsev[ii]);
      }
      //	      	m_vPrev[dim]->m_vHalf[gi][ii] = C1_up/C1_down*fast_exp(gasv[gi]->getMass(), T, impulsev[ii]);
      m_vValue[gi][ii] = 2 * C2_up / C1_down*fast_exp(gasv[gi]->getMass(), m_vBoundaryTemperature[gi], impulsev[ii]) - compute_av(dim, gi, ii, AD_PREV);
      if (m_vValue[gi][ii] < 0.0) m_vValue[gi][ii] = 0.0;
    }
  }
}

/* Setting gases boundary type. */
void Cell::compute_half_gase_left(unsigned int dim, int gi) {
	GasVector& gasv = Config::vGas;
	Impulse* pImpulse = m_pSolver->GetImpulse();
	ImpulseVector& impulsev = pImpulse->getVector();

	double C1_up = 0.0;
	double C1_down = 0.0;
	double C2_up = 0.0;

	Vector3d v3Speed = Vector3d();
	if (m_vBoundaryPressure[gi] > 0.0) {
		v3Speed = m_vBoundaryStream[gi] / (m_vBoundaryPressure[gi] / m_vBoundaryTemperature[gi]);
	}

	for (unsigned int ii = 0; ii<impulsev.size(); ii++) {
		if (impulsev[ii][dim] < 0) {
			double y = Config::dTimestep / gasv[gi]->getMass()*std::abs(impulsev[ii][dim] / m_vAreastep[dim]);

			m_vValue[gi][ii] = 2 * compute_av(dim, gi, ii, AD_NEXT) - compute_av(dim, gi, ii, AD_NEXTNEXT);
			if (m_vValue[gi][ii] < 0) m_vValue[gi][ii] = 0;

			m_vHalf[gi][ii] = compute_av(dim, gi, ii, AD_NEXT) - (1 - y) / 2 * limiter(
				m_vValue[gi][ii],
				compute_av(dim, gi, ii, AD_NEXT),
				compute_av(dim, gi, ii, AD_NEXTNEXT));

			C1_up += m_vHalf[gi][ii];
			C2_up += (m_vValue[gi][ii] + compute_av(dim, gi, ii, AD_NEXT)) / 2;
		}
		else {
			C1_down += fast_exp(
				gasv[gi]->getMass(), 
				m_vBoundaryTemperature[gi], 
				impulsev[ii] - v3Speed * gasv[gi]->getMass());
		}
	}

	// Vacuum
	if (m_vBoundaryPressure[gi] == 0.0) {
		C1_up = 0.0;
		C2_up = 0.0;
	}
	else {
		C1_up = m_vBoundaryPressure[gi] / m_vBoundaryTemperature[gi] / pImpulse->getDeltaImpulseQube() - C1_up;
		C2_up = m_vBoundaryPressure[gi] / m_vBoundaryTemperature[gi] / pImpulse->getDeltaImpulseQube() - C2_up;

		if (C1_up < 0.0) C1_up = 0.0;
		if (C2_up < 0.0) C2_up = 0.0;
	}

	for (unsigned int ii = 0; ii<impulsev.size(); ii++) {
		if (impulsev[ii][dim] > 0) {
			m_vHalf[gi][ii] = C1_up / C1_down * fast_exp(
				gasv[gi]->getMass(), 
				m_vBoundaryTemperature[gi], 
				impulsev[ii] - v3Speed * gasv[gi]->getMass());

			m_vValue[gi][ii] = 2 * C2_up / C1_down * fast_exp(
				gasv[gi]->getMass(), 
				m_vBoundaryTemperature[gi], 
				impulsev[ii] - v3Speed * gasv[gi]->getMass()) - compute_av(dim, gi, ii, AD_NEXT);
			if (m_vValue[gi][ii] < 0.0) m_vValue[gi][ii] = 0.0;
		}
	}
}

void Cell::compute_half_gase_right(unsigned int dim, int gi) {
	GasVector& gasv = Config::vGas;
	Impulse* pImpulse = m_pSolver->GetImpulse();
	ImpulseVector& impulsev = pImpulse->getVector();

	double C1_up = 0.0;
	double C1_down = 0.0;
	double C2_up = 0.0;

	Vector3d v3Speed = Vector3d();
	if (m_vBoundaryPressure[gi] > 0.0) {
		v3Speed = m_vBoundaryStream[gi] / (m_vBoundaryPressure[gi] / m_vBoundaryTemperature[gi]);
	}

	for (unsigned int ii = 0; ii<impulsev.size(); ii++) {
		if (impulsev[ii][dim] > 0) {
			double y = Config::dTimestep / gasv[gi]->getMass()*std::abs(impulsev[ii][dim] / m_vAreastep[dim]);

			m_vValue[gi][ii] = 2 * compute_av(dim, gi, ii, AD_PREV) - compute_av(dim, gi, ii, AD_PREVPREV);
			if (m_vValue[gi][ii] < 0) m_vValue[gi][ii] = 0;

			for (auto& item : m_vPrev[dim]) {
				item->m_vHalf[gi][ii] = item->m_vValue[gi][ii] + (1 - y) / 2 * limiter(
					item->compute_av(dim, gi, ii, AD_PREV),
					item->m_vValue[gi][ii],
					item->compute_av(dim, gi, ii, AD_NEXT));
			}

			C1_up += compute_ah(dim, gi, ii, AD_PREV);
			C2_up += (m_vValue[gi][ii] + compute_av(dim, gi, ii, AD_PREV)) / 2;
		}
		else {
			C1_down += fast_exp(
				gasv[gi]->getMass(), 
				m_vBoundaryTemperature[gi], 
				impulsev[ii] - v3Speed * gasv[gi]->getMass());
		}
	}

	// Vacuum
	if (m_vBoundaryPressure[gi] == 0.0) {
		C1_up = 0.0;
		C2_up = 0.0;
	}
	else {
		C1_up = m_vBoundaryPressure[gi] / m_vBoundaryTemperature[gi] / pImpulse->getDeltaImpulseQube() - C1_up;
		C2_up = m_vBoundaryPressure[gi] / m_vBoundaryTemperature[gi] / pImpulse->getDeltaImpulseQube() - C2_up;

		if (C1_up < 0.0) C1_up = 0.0;
		if (C2_up < 0.0) C2_up = 0.0;
	}

	for (unsigned int ii = 0; ii<impulsev.size(); ii++) {
		if (impulsev[ii][dim] < 0) {
			for (auto& item : m_vPrev[dim]) {
				item->m_vHalf[gi][ii] = C1_up / C1_down * fast_exp(
					gasv[gi]->getMass(), 
					m_vBoundaryTemperature[gi], 
					impulsev[ii] - v3Speed * gasv[gi]->getMass());
			}

			m_vValue[gi][ii] = 2 * C2_up / C1_down * fast_exp(
				gasv[gi]->getMass(), 
				m_vBoundaryTemperature[gi], 
				impulsev[ii] - v3Speed * gasv[gi]->getMass()) - compute_av(dim, gi, ii, AD_PREV);
			if (m_vValue[gi][ii] < 0.0) m_vValue[gi][ii] = 0.0;
		}
	}
}

/* Setting flow boundary type. */
void Cell::compute_half_flow_left(unsigned int dim, int gi) {
	GasVector& gasv = Config::vGas;
	Impulse* pImpulse = m_pSolver->GetImpulse();
	ImpulseVector& impulsev = pImpulse->getVector();

	double C1_up = 0.0;
	double C1_down = 0.0;
	double C2_up = 0.0;

	for (unsigned int ii = 0; ii<impulsev.size(); ii++) {
		if (impulsev[ii][dim] < 0) {
			double y = Config::dTimestep / gasv[gi]->getMass()*std::abs(impulsev[ii][dim] / m_vAreastep[dim]);

			m_vValue[gi][ii] = 2 * compute_av(dim, gi, ii, AD_NEXT) - compute_av(dim, gi, ii, AD_NEXTNEXT);
			if (m_vValue[gi][ii] < 0) m_vValue[gi][ii] = 0;

			m_vHalf[gi][ii] = compute_av(dim, gi, ii, AD_NEXT) - (1 - y) / 2 * limiter(
				m_vValue[gi][ii],
				compute_av(dim, gi, ii, AD_NEXT),
				compute_av(dim, gi, ii, AD_NEXTNEXT));

			C1_up += impulsev[ii][dim] * m_vHalf[gi][ii];
			C2_up += impulsev[ii][dim] * (m_vValue[gi][ii] + compute_av(dim, gi, ii, AD_NEXT)) / 2;
		}
		else {
			C1_down += impulsev[ii][dim] * fast_exp(
				gasv[gi]->getMass(),
				m_vBoundaryTemperature[gi],
				impulsev[ii]);
		}
	}

	C1_up = m_vBoundaryStream[gi][dim] / pImpulse->getDeltaImpulseQube() - C1_up;
	C2_up = m_vBoundaryStream[gi][dim] / pImpulse->getDeltaImpulseQube() - C2_up;

	if (C1_up / C1_down < 0.0) C1_up = 0.0;
	if (C2_up / C1_down < 0.0) C2_up = 0.0;
	
	for (unsigned int ii = 0; ii<impulsev.size(); ii++) {
		if (impulsev[ii][dim] > 0) {
			m_vHalf[gi][ii] = C1_up / C1_down * fast_exp(
				gasv[gi]->getMass(),
				m_vBoundaryTemperature[gi],
				impulsev[ii]);

			m_vValue[gi][ii] = 2 * C2_up / C1_down * fast_exp(
				gasv[gi]->getMass(),
				m_vBoundaryTemperature[gi],
				impulsev[ii]) - compute_av(dim, gi, ii, AD_NEXT);
			if (m_vValue[gi][ii] < 0.0) m_vValue[gi][ii] = 0.0;
		}
	}
}

void Cell::compute_half_flow_right(unsigned int dim, int gi) {
	GasVector& gasv = Config::vGas;
	Impulse* pImpulse = m_pSolver->GetImpulse();
	ImpulseVector& impulsev = pImpulse->getVector();

	double C1_up = 0.0;
	double C1_down = 0.0;
	double C2_up = 0.0;

	for (unsigned int ii = 0; ii<impulsev.size(); ii++) {
		if (impulsev[ii][dim] > 0) {
			double y = Config::dTimestep / gasv[gi]->getMass()*std::abs(impulsev[ii][dim] / m_vAreastep[dim]);

			m_vValue[gi][ii] = 2 * compute_av(dim, gi, ii, AD_PREV) - compute_av(dim, gi, ii, AD_PREVPREV);
			if (m_vValue[gi][ii] < 0) m_vValue[gi][ii] = 0;

			for (auto& item : m_vPrev[dim]) {
				item->m_vHalf[gi][ii] = item->m_vValue[gi][ii] + (1 - y) / 2 * limiter(
					item->compute_av(dim, gi, ii, AD_PREV),
					item->m_vValue[gi][ii],
					item->compute_av(dim, gi, ii, AD_NEXT));
			}

			C1_up += std::abs(impulsev[ii][dim]) * compute_ah(dim, gi, ii, AD_PREV);
			C2_up += std::abs(impulsev[ii][dim]) * (m_vValue[gi][ii] + compute_av(dim, gi, ii, AD_PREV)) / 2;
		}
		else {
			C1_down += std::abs(impulsev[ii][dim]) * fast_exp(
				gasv[gi]->getMass(),
				m_vBoundaryTemperature[gi],
				impulsev[ii]);
		}
	}

	C1_up = m_vBoundaryStream[gi][dim] / pImpulse->getDeltaImpulseQube() - C1_up;
	C2_up = m_vBoundaryStream[gi][dim] / pImpulse->getDeltaImpulseQube() - C2_up;

	if (C1_up / C1_down < 0.0) C1_up = 0.0;
	if (C2_up / C1_down < 0.0) C2_up = 0.0;

	for (unsigned int ii = 0; ii<impulsev.size(); ii++) {
		if (impulsev[ii][dim] < 0) {
			for (auto& item : m_vPrev[dim]) {
				item->m_vHalf[gi][ii] = C1_up / C1_down * fast_exp(
					gasv[gi]->getMass(),
					m_vBoundaryTemperature[gi],
					impulsev[ii]);
			}

			m_vValue[gi][ii] = 2 * C2_up / C1_down * fast_exp(
				gasv[gi]->getMass(),
				m_vBoundaryTemperature[gi],
				impulsev[ii]) - compute_av(dim, gi, ii, AD_PREV);
			if (m_vValue[gi][ii] < 0.0) m_vValue[gi][ii] = 0.0;
		}
	}
}

/* Setting mirror boundary type. */
void Cell::compute_half_mirror_left(unsigned int dim, int gi)
{
  GasVector& gasv = Config::vGas;
  Impulse* impulse = m_pSolver->GetImpulse();
  ImpulseVector& impulsev = impulse->getVector();

  for (unsigned int ii = 0; ii<impulsev.size(); ii++) {
    double y = Config::dTimestep / gasv[gi]->getMass()*std::abs(impulsev[ii][dim] / m_vAreastep[dim]);

    if (impulsev[ii][dim] > 0) {
      int ri = impulse->reverseIndex(ii, static_cast<sep::Axis>(dim));
      
      m_vHalf[gi][ii] = m_vValue[gi][ii] + (1 - y) / 2 * limiter(
        m_vValue[gi][ri], // reversed for left
        m_vValue[gi][ii],
        compute_av(dim, gi, ii, AD_NEXT)); // requires reverse MT_END
    }
    else {
      m_vHalf[gi][ii] = compute_av(dim, gi, ii, AD_NEXT) - (1 - y) / 2 * limiter(
        m_vValue[gi][ii],
        compute_av(dim, gi, ii, AD_NEXT), // requires reverse MT_END
        compute_av(dim, gi, ii, AD_NEXTNEXT)); // requires reverse MT_END
    }
  }

  for (unsigned int ii = 0; ii < impulsev.size(); ii++) {
    double y = Config::dTimestep / gasv[gi]->getMass()*impulsev[ii][dim] / m_vAreastep[dim];

    int ri = impulse->reverseIndex(ii, static_cast<sep::Axis>(dim));
    //std::cout << "Boundary type = " << dim << std::endl;

    m_vValue[gi][ii] = m_vValue[gi][ii] - y*(m_vHalf[gi][ii] - m_vHalf[gi][ri]); // requires reverse MT_BEGIN
  }
}

void Cell::compute_half_mirror_right(unsigned int dim, int gi)
{
  GasVector& gasv = Config::vGas;
  Impulse* pImpulse = m_pSolver->GetImpulse();
  ImpulseVector& impulsev = pImpulse->getVector();

  for (unsigned int ii = 0; ii<impulsev.size(); ii++) {
    double y = Config::dTimestep / gasv[gi]->getMass()*std::abs(impulsev[ii][dim] / m_vAreastep[dim]);

    int ri = pImpulse->reverseIndex(ii, static_cast<sep::Axis>(dim));
      
    if (impulsev[ii][dim] > 0) {
      // PREV
      for (auto& item : m_vPrev[dim]) {
        item->m_vHalf[gi][ii] = item->m_vValue[gi][ii] + (1 - y) / 2 * limiter(
          item->compute_av(dim, gi, ii, AD_PREV),
          item->m_vValue[gi][ii],
          item->compute_av(dim, gi, ii, AD_NEXT));
      }

      // CURRENT
      m_vHalf[gi][ii] = m_vValue[gi][ii] + (1 - y) / 2 * limiter(
        compute_av(dim, gi, ii, AD_PREV), // requires reverse MT_BEGIN
        m_vValue[gi][ii],
        m_vValue[gi][ri]); // reversed for right
    }
    else {
      // PREV
      for (auto& item : m_vPrev[dim]) {
        item->m_vHalf[gi][ii] = item->compute_av(dim, gi, ii, AD_NEXT) - (1 - y) / 2 * limiter(
          item->m_vValue[gi][ii],
          item->compute_av(dim, gi, ii, AD_NEXT), // requires reverse MT_END
          item->compute_av(dim, gi, ri, AD_NEXT)); // requires reverse MT_END
      }

      // CURRENT
      m_vHalf[gi][ii] = m_vValue[gi][ri] - (1 - y) / 2 * limiter(
        m_vValue[gi][ii],
        m_vValue[gi][ri], // requires reverse MT_END
        compute_av(dim, gi, ri, AD_PREV)); // requires reverse MT_END
    }
  }

  for (unsigned int ii = 0; ii < impulsev.size(); ii++) {
    double y = Config::dTimestep / gasv[gi]->getMass()*impulsev[ii][dim] / m_vAreastep[dim]; // requires reverse MT_BEGIN

    m_vValue[gi][ii] = m_vValue[gi][ii] - y*(m_vHalf[gi][ii] - compute_ah(dim, gi, ii, AD_PREV)); // requires reverse MT_BEGIN
  }
}
// HELP METHODS
inline void Cell::cycleValue(double& result, unsigned int& count, const unsigned int& gi,
  const unsigned int& ii, const Cell::CellVector& cellVector)
{
  for (auto& item : cellVector) {
    result += item->m_vValue[gi][ii];
    count++;
  }
}

inline void Cell::cycleHalf(double& result, unsigned int& count, const unsigned int& gi,
  const unsigned int& ii, const Cell::CellVector& cellVector)
{
  for (auto& item : cellVector) {
    result += item->m_vHalf[gi][ii];
    count++;
  }
}

double Cell::compute_av(unsigned int dim, unsigned int gi, unsigned int ii, AverageDepth eAverageDepth) {
  double result = 0.0;
  unsigned int count = 0;
  switch (eAverageDepth) {
  case AD_NEXT:
    cycleValue(result, count, gi, ii, m_vNext[dim]);
    break;
  case AD_NEXTNEXT:
    for (auto& item1 : m_vNext[dim]) {
      cycleValue(result, count, gi, ii, item1->m_vNext[dim]);
    }
    break;
  case AD_PREV:
    cycleValue(result, count, gi, ii, m_vPrev[dim]);
    break;
  case AD_PREVPREV:
    for (auto& item1 : m_vPrev[dim]) {
      cycleValue(result, count, gi, ii, item1->m_vPrev[dim]);
    }
    break;
  }

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
    cycleHalf(result, count, gi, ii, m_vNext[dim]);
    break;
  case AD_NEXTNEXT:
    for (auto& item1 : m_vNext[dim]) {
      cycleHalf(result, count, gi, ii, item1->m_vNext[dim]);
    }
    break;
  case AD_PREV:
    cycleHalf(result, count, gi, ii, m_vPrev[dim]);
    break;
  case AD_PREVPREV:
    for (auto& item1 : m_vPrev[dim]) {
      cycleHalf(result, count, gi, ii, item1->m_vPrev[dim]);
    }
    break;
  }
  if (count == 0 || count > 4)
    std::cout << "Half" << eAverageDepth << " : " << count << " Type = " << m_vType[dim] << std::endl;
  result /= count;
  return result;
}

double Cell::fast_exp(const double& mass, const double& temp, const Vector3d& impulse) {
  return std::exp(-impulse.mod2() / mass / 2 / temp);
}

double Cell::limiter_superbee(const double& x, const double& y, const double& z) {
  if ((z - y)*(y - x) <= 0) return 0.0;
  else return std::max(0.0, std::min(2 * std::abs(y - x), std::min(std::abs(z - y), std::min(std::abs(y - x), 2 * std::abs(z - y)))))*sgn(z - y);
}

/* Macro Data */

double Cell::compute_concentration(unsigned int gi) {
  Impulse* impulse = m_pSolver->GetImpulse();
  ImpulseVector& impulsev = impulse->getVector();

  double concentration = 0.0;
  for (unsigned int ii = 0; ii < impulsev.size(); ii++) {
    concentration += m_vValue[gi][ii];
  }
  concentration *= impulse->getDeltaImpulseQube();
  return concentration;
}

double Cell::compute_temperature(unsigned int gi) {
  GasVector& gasv = Config::vGas;
  Impulse* impulse = m_pSolver->GetImpulse();
  ImpulseVector& impulsev = impulse->getVector();

  double concentration = m_vMacroData[gi].C;
  double temperature = 0.0;
  Vector3d vAverageSpeed = m_vMacroData[gi].Stream;
  vAverageSpeed /= concentration;

  for (unsigned int ii = 0; ii < impulsev.size(); ii++) {
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

double Cell::compute_pressure(unsigned int gi)
{
  return m_vMacroData[gi].C * m_vMacroData[gi].T;
}

Vector3d Cell::compute_stream(unsigned int gi) {
  GasVector& gasv = Config::vGas;
  Impulse* impulse = m_pSolver->GetImpulse();
  ImpulseVector& impulsev = impulse->getVector();

  Vector3d vStream;
  for (unsigned int ii = 0; ii < impulsev.size(); ii++) {
    for (unsigned int vi = 0; vi < vStream.getMass().size(); vi++) {
      vStream[vi] += impulsev[ii][vi] * m_vValue[gi][ii];
    }
  }
  vStream *= impulse->getDeltaImpulseQube() / gasv[gi]->getMass();

  return vStream;
}

Vector3d Cell::compute_heatstream(unsigned int gi) {
  return Vector3d();
}
