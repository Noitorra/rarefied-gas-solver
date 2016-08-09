#include "cell.h"

#include "grid_manager.h"
#include "solver.h"
#include "parameters/gas.h"
#include "parameters/impulse.h"
#include "config/config.h"

#include "integral/ci.hpp"

Cell::Cell() :
        m_pGridManager(nullptr),
        m_pSolver(nullptr),
        m_pGrid(nullptr),
        m_pConfig(Config::getInstance()),
        m_iLockedAxes(-1) {

    m_vType.resize(3, Type::UNDEFINED);

    // Create 3 std::vector< Cell* > for dimentions
    m_pNext.resize(3, nullptr);
    m_pPrev.resize(3, nullptr);

    const unsigned int gasesNum = m_pConfig->getGasesNum();

    m_vStartTemperature.resize(gasesNum);
    m_vStartPressure.resize(gasesNum);

    m_vBoundaryType.resize(gasesNum);
    m_vBoundaryTemperature.resize(gasesNum);
    m_vBoundaryStream.resize(gasesNum);
    m_vBoundaryPressure.resize(gasesNum);
}

void Cell::Init(GridManager* pGridManager) {
    m_pGridManager = pGridManager;
    m_pSolver = pGridManager->GetSolver();
    m_pGrid = pGridManager->GetGrid();

    const GasVector& vGases = m_pConfig->getGases();
    int iGasesNum = m_pConfig->getGasesNum();
    Impulse* pImpulse = m_pSolver->GetImpulse();
    ImpulseVector& vImpulses = pImpulse->getVector();

    // Allocating space for values and half's
    m_vHalf.resize(iGasesNum);
    m_vValue.resize(iGasesNum);
    m_vMacroData.resize(iGasesNum);

    for (int gi = 0; gi < iGasesNum; gi++) {
        m_vHalf[gi].resize(vImpulses.size(), 0.0);
        m_vValue[gi].resize(vImpulses.size(), 0.0);

        double dDensity = 0.0;
        for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
            dDensity += compute_exp(vGases[gi]->getMass(), m_vStartTemperature[gi], vImpulses[ii]);
        }

        dDensity *= pImpulse->getDeltaImpulseQube();
        dDensity = m_vStartPressure[gi] / m_vStartTemperature[gi] / dDensity;

        for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
            m_vValue[gi][ii] = dDensity * compute_exp(vGases[gi]->getMass(), m_vStartTemperature[gi], vImpulses[ii]);
        }
    }
}

void Cell::setParameters(double dPressure, double dTemperature, Vector3d dAreastep, int iGasIndex, int iLockedAxes) {
    m_vStartPressure[iGasIndex] = dPressure;
    m_vStartTemperature[iGasIndex] = dTemperature;
    m_vAreastep = dAreastep;
    m_iLockedAxes = iLockedAxes;
}

void Cell::setBoundaryType(sep::BoundaryType eBoundaryType, double dTemperature, Vector3d dStream, double dPressure, int iGasIndex) {
    m_vBoundaryType[iGasIndex] = eBoundaryType;
    m_vBoundaryTemperature[iGasIndex] = dTemperature;
    m_vBoundaryStream[iGasIndex] = dStream;
    m_vBoundaryPressure[iGasIndex] = dPressure;
}

void Cell::computeType(unsigned int dim) {
    compute_type(dim);
}

void Cell::computeHalf(unsigned int dim) {
    if (dim == m_iLockedAxes)
        return;

    //std::cout << "Type: " << m_vType[dim] << std::endl;
    switch (m_vType[dim]) {
        case Type::LEFT:
            compute_half_left(dim);
            break;
        case Type::NORMAL:
            compute_half_normal(dim);
            break;
        case Type::PRERIGHT:
            compute_half_preright(dim);
            break;
        case Type::RIGHT:
            compute_half_right(dim);
            break;
        default:
            break;
    }
}

void Cell::computeValue(unsigned int dim) {
    if (dim == m_iLockedAxes)
        return;

    switch (m_vType[dim]) {
        case Type::NORMAL:
            compute_value_normal(dim);
            break;
        case Type::PRERIGHT:
            compute_value_normal(dim);
            break;
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
        dTempValue = m_vValue[gi0][ii] * lambda * m_pConfig->getTimestep(); // TODO: check lambda * Config::m_dTimestep < 1 !!!
        m_vValue[gi0][ii] -= dTempValue;
        m_vValue[gi1][ii] += dTempValue;
    }
}

void Cell::computeMacroData() {
    int iGasesNum = m_pConfig->getGasesNum();

    if (!is_normal()) {
        for (int gi = 0; gi < iGasesNum; gi++) {
            m_vMacroData[gi].dDensity = 0.0;
            m_vMacroData[gi].Stream = Vector3d();
            m_vMacroData[gi].dTemperature = 0.0;
            m_vMacroData[gi].dPressure = 0.0;
            m_vMacroData[gi].HeatStream = Vector3d();
        }
    } else {
        for (int gi = 0; gi < iGasesNum; gi++) {
            m_vMacroData[gi].dDensity = compute_concentration(gi);
            m_vMacroData[gi].Stream = compute_stream(gi);
            m_vMacroData[gi].dTemperature = (m_vMacroData[gi].dDensity == 0.0) ? 0.0 : compute_temperature(gi);
            m_vMacroData[gi].dPressure = (m_vMacroData[gi].dDensity == 0.0) ? 0.0 : compute_pressure(gi);
            m_vMacroData[gi].HeatStream = compute_heatstream(gi);
        }
    }
}

bool Cell::is_normal() {
    bool notX = m_vType[sep::X] == Type::LEFT || m_vType[sep::X] == Type::RIGHT;
    bool notY = m_vType[sep::Y] == Type::LEFT || m_vType[sep::Y] == Type::RIGHT;
    bool notZ = m_vType[sep::Z] == Type::LEFT || m_vType[sep::Z] == Type::RIGHT;
    return !notX && !notY && !notZ;
}

/* Tests */
bool Cell::checkInnerValuesRange() {
    int iGasesNum = m_pConfig->getGasesNum();
    ImpulseVector& vImpulses = m_pSolver->GetImpulse()->getVector();

    bool result = true;

    for (unsigned int gi = 0; gi < iGasesNum; gi++) {
        for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
            if (m_vValue[gi][ii] < 0) {
                // wrong values
                result = false;
                std::cout << "Cell::checkInnerValuesRange(): Error [type][gi][ii]"
                          << "[" << as_integer(m_vType[0]) << ":" << as_integer(m_vType[1]) << ":" << as_integer(m_vType[2]) << "]"
                          << "[" << gi << "]"
                          << "[" << ii << "]"
                          << " value = " << m_vValue[gi][ii] << std::endl;
            }
            if (m_vHalf[gi][ii] < 0) {
                // wrong values
                result = false;
                std::cout << "Cell::checkInnerValuesRange(): Error [type][gi][ii]"
                          << "[" << as_integer(m_vType[0]) << ":" << as_integer(m_vType[1]) << ":" << as_integer(m_vType[2]) << "]"
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
    if (m_pPrev[dim] == nullptr) {
        if (m_pNext[dim] != nullptr) {
            m_vType[dim] = Type::LEFT;
        } else {
            //std::cout << "Cell has no neighbors in dim = " << dim << std::endl;
        }
    } else {
        if (m_pNext[dim] == nullptr) {
            m_vType[dim] = Type::RIGHT;
        } else {
            if (m_pNext[dim]->m_pNext[dim] == nullptr) {
                m_vType[dim] = Type::PRERIGHT;
            } else {
                m_vType[dim] = Type::NORMAL;
            }
        }
    }

    if (m_vType[dim] == Type::UNDEFINED) {
        //std::cout << "Cannot find this cell type, maybe cells are not linked, or linked wrong." << std::endl;
    }
}

void Cell::compute_half_left(unsigned int dim) {
    int iGasesNum = m_pConfig->getGasesNum();

    for (int gi = 0; gi < iGasesNum; gi++) {
        switch (m_vBoundaryType[gi]) {
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
    const GasVector& vGases = m_pConfig->getGases();
    int iGasesNum = m_pConfig->getGasesNum();
    ImpulseVector& vImpulses = m_pSolver->GetImpulse()->getVector();

    for (unsigned int gi = 0; gi < iGasesNum; gi++) {
        for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
            double y = m_pConfig->getTimestep() / vGases[gi]->getMass() * std::abs(vImpulses[ii][dim] / m_vAreastep[dim]);

            if (vImpulses[ii][dim] > 0) {
                m_vHalf[gi][ii] = m_vValue[gi][ii] + (1 - y) / 2 * limiter(
                        m_pPrev[dim]->m_vValue[gi][ii],
                        m_vValue[gi][ii],
                        m_pNext[dim]->m_vValue[gi][ii]);
            } else {
                m_vHalf[gi][ii] = m_pNext[dim]->m_vValue[gi][ii] - (1 - y) / 2 * limiter(
                        m_vValue[gi][ii],
                        m_pNext[dim]->m_vValue[gi][ii],
                        m_pNext[dim]->m_pNext[dim]->m_vValue[gi][ii]);
            }
        }
    }
}

void Cell::compute_half_preright(unsigned int dim) {}

void Cell::compute_half_right(unsigned int dim) {
    int iGasesNum = m_pConfig->getGasesNum();

    for (int gi = 0; gi < iGasesNum; gi++) {
        switch (m_vBoundaryType[gi]) {
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
    const GasVector& vGases = m_pConfig->getGases();
    int iGasesNum = m_pConfig->getGasesNum();
    ImpulseVector& vImpulses = m_pSolver->GetImpulse()->getVector();

    for (unsigned int gi = 0; gi < iGasesNum; gi++) {
        for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
            double y = m_pConfig->getTimestep() / vGases[gi]->getMass() * vImpulses[ii][dim] / m_vAreastep[dim];

            m_vValue[gi][ii] = m_vValue[gi][ii] - y * (m_vHalf[gi][ii] - m_pPrev[dim]->m_vHalf[gi][ii]);
        }
    }
}

/* Diffuse boundary type. */
void Cell::compute_half_diffuse_left(unsigned int dim, int gi) {
    const GasVector& vGases = m_pConfig->getGases();
    ImpulseVector& vImpulses = m_pSolver->GetImpulse()->getVector();

    double C1_up = 0.0;
    double C1_down = 0.0;
    double C2_up = 0.0;
    for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
        if (vImpulses[ii][dim] < 0) {
            double y = m_pConfig->getTimestep() / vGases[gi]->getMass() * std::abs(vImpulses[ii][dim] / m_vAreastep[dim]);

            m_vValue[gi][ii] = 2 * m_pNext[dim]->m_vValue[gi][ii] - m_pNext[dim]->m_pNext[dim]->m_vValue[gi][ii];
            if (m_vValue[gi][ii] < 0)
                m_vValue[gi][ii] = 0;

            m_vHalf[gi][ii] = m_pNext[dim]->m_vValue[gi][ii] - (1 - y) / 2 * limiter(
                    m_vValue[gi][ii],
                    m_pNext[dim]->m_vValue[gi][ii],
                    m_pNext[dim]->m_pNext[dim]->m_vValue[gi][ii]);

            C1_up += std::abs(vImpulses[ii][dim] * m_vHalf[gi][ii]);
            C2_up += std::abs(vImpulses[ii][dim] * (m_vValue[gi][ii] + m_pNext[dim]->m_vValue[gi][ii]) / 2);
        } else {
            C1_down += std::abs(vImpulses[ii][dim] * compute_exp(vGases[gi]->getMass(), m_vBoundaryTemperature[gi], vImpulses[ii]));
        }
    }

    for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
        if (vImpulses[ii][dim] > 0) {
            m_vHalf[gi][ii] = C1_up / C1_down * compute_exp(vGases[gi]->getMass(), m_vBoundaryTemperature[gi], vImpulses[ii]);
            m_vValue[gi][ii] = 2 * C2_up / C1_down * compute_exp(vGases[gi]->getMass(), m_vBoundaryTemperature[gi], vImpulses[ii]) - m_pNext[dim]->m_vValue[gi][ii];
            if (m_vValue[gi][ii] < 0.0)
                m_vValue[gi][ii] = 0.0;
        }
    }
}

void Cell::compute_half_diffuse_right(unsigned int dim, int gi) {
    const GasVector& vGases = m_pConfig->getGases();
    ImpulseVector& vImpulses = m_pSolver->GetImpulse()->getVector();

    double C1_up = 0.0;
    double C1_down = 0.0;
    double C2_up = 0.0;
    for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
        if (vImpulses[ii][dim] > 0) {
            double y = m_pConfig->getTimestep() / vGases[gi]->getMass() * std::abs(vImpulses[ii][dim] / m_vAreastep[dim]);

            m_vValue[gi][ii] = 2 * m_pPrev[dim]->m_vValue[gi][ii] - m_pPrev[dim]->m_pPrev[dim]->m_vValue[gi][ii];
            if (m_vValue[gi][ii] < 0)
                m_vValue[gi][ii] = 0;

            m_pPrev[dim]->m_vHalf[gi][ii] = m_pPrev[dim]->m_vValue[gi][ii] + (1 - y) / 2 * limiter(
                    m_pPrev[dim]->m_pPrev[dim]->m_vValue[gi][ii],
                    m_pPrev[dim]->m_vValue[gi][ii],
                    m_vValue[gi][ii]);

            C1_up += std::abs(vImpulses[ii][dim] * m_pPrev[dim]->m_vHalf[gi][ii]);
            C2_up += std::abs(vImpulses[ii][dim] * (m_vValue[gi][ii] + m_pPrev[dim]->m_vValue[gi][ii]) / 2);
        } else {
            C1_down += std::abs(vImpulses[ii][dim] * compute_exp(vGases[gi]->getMass(), m_vBoundaryTemperature[gi], vImpulses[ii]));
        }
    }

    for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
        if (vImpulses[ii][dim] < 0) {
            m_pPrev[dim]->m_vHalf[gi][ii] = C1_up / C1_down * compute_exp(vGases[gi]->getMass(), m_vBoundaryTemperature[gi], vImpulses[ii]);
            m_vValue[gi][ii] = 2 * C2_up / C1_down * compute_exp(vGases[gi]->getMass(), m_vBoundaryTemperature[gi], vImpulses[ii]) - m_pPrev[dim]->m_vValue[gi][ii];
            if (m_vValue[gi][ii] < 0.0)
                m_vValue[gi][ii] = 0.0;
        }
    }
}

/* Setting gases boundary type. */
void Cell::compute_half_gase_left(unsigned int dim, int gi) {
    const GasVector& vGases = m_pConfig->getGases();
    Impulse* pImpulse = m_pSolver->GetImpulse();
    ImpulseVector& vImpulses = pImpulse->getVector();

    double C1_up = 0.0;
    double C1_down = 0.0;
    double C2_up = 0.0;

    Vector3d v3Speed = Vector3d();
    if (m_vBoundaryPressure[gi] > 0.0) {
        v3Speed = m_vBoundaryStream[gi] / (m_vBoundaryPressure[gi] / m_vBoundaryTemperature[gi]);
    }

    for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
        if (vImpulses[ii][dim] < 0) {
            double y = m_pConfig->getTimestep() / vGases[gi]->getMass() * std::abs(vImpulses[ii][dim] / m_vAreastep[dim]);

            m_vValue[gi][ii] = 2 * m_pNext[dim]->m_vValue[gi][ii] - m_pNext[dim]->m_pNext[dim]->m_vValue[gi][ii];
            if (m_vValue[gi][ii] < 0)
                m_vValue[gi][ii] = 0;

            m_vHalf[gi][ii] = m_pNext[dim]->m_vValue[gi][ii] - (1 - y) / 2 * limiter(
                    m_vValue[gi][ii],
                    m_pNext[dim]->m_vValue[gi][ii],
                    m_pNext[dim]->m_pNext[dim]->m_vValue[gi][ii]);

            C1_up += m_vHalf[gi][ii];
            C2_up += (m_vValue[gi][ii] + m_pNext[dim]->m_vValue[gi][ii]) / 2;
        } else {
            C1_down += compute_exp(
                    vGases[gi]->getMass(),
                    m_vBoundaryTemperature[gi],
                    vImpulses[ii] - v3Speed * vGases[gi]->getMass());
        }
    }

    // Vacuum
    if (m_vBoundaryPressure[gi] == 0.0) {
        C1_up = 0.0;
        C2_up = 0.0;
    } else {
        C1_up = m_vBoundaryPressure[gi] / m_vBoundaryTemperature[gi] / pImpulse->getDeltaImpulseQube() - C1_up;
        C2_up = m_vBoundaryPressure[gi] / m_vBoundaryTemperature[gi] / pImpulse->getDeltaImpulseQube() - C2_up;

        if (C1_up < 0.0)
            C1_up = 0.0;
        if (C2_up < 0.0)
            C2_up = 0.0;
    }

    for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
        if (vImpulses[ii][dim] > 0) {
            m_vHalf[gi][ii] = C1_up / C1_down * compute_exp(
                    vGases[gi]->getMass(),
                    m_vBoundaryTemperature[gi],
                    vImpulses[ii] - v3Speed * vGases[gi]->getMass());

            m_vValue[gi][ii] = 2 * C2_up / C1_down * compute_exp(
                    vGases[gi]->getMass(),
                    m_vBoundaryTemperature[gi],
                    vImpulses[ii] - v3Speed * vGases[gi]->getMass()) - m_pNext[dim]->m_vValue[gi][ii];
            if (m_vValue[gi][ii] < 0.0)
                m_vValue[gi][ii] = 0.0;
        }
    }
}

void Cell::compute_half_gase_right(unsigned int dim, int gi) {
    const GasVector& vGases = m_pConfig->getGases();
    Impulse* pImpulse = m_pSolver->GetImpulse();
    ImpulseVector& vImpulses = pImpulse->getVector();

    double C1_up = 0.0;
    double C1_down = 0.0;
    double C2_up = 0.0;

    Vector3d v3Speed = Vector3d();
    if (m_vBoundaryPressure[gi] > 0.0) {
        v3Speed = m_vBoundaryStream[gi] / (m_vBoundaryPressure[gi] / m_vBoundaryTemperature[gi]);
    }

    for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
        if (vImpulses[ii][dim] > 0) {
            double y = m_pConfig->getTimestep() / vGases[gi]->getMass() * std::abs(vImpulses[ii][dim] / m_vAreastep[dim]);

            m_vValue[gi][ii] = 2 * m_pPrev[dim]->m_vValue[gi][ii] - m_pPrev[dim]->m_pPrev[dim]->m_vValue[gi][ii];
            if (m_vValue[gi][ii] < 0)
                m_vValue[gi][ii] = 0;

            m_pPrev[dim]->m_vHalf[gi][ii] = m_pPrev[dim]->m_vValue[gi][ii] + (1 - y) / 2 * limiter(
                    m_pPrev[dim]->m_pPrev[dim]->m_vValue[gi][ii],
                    m_pPrev[dim]->m_vValue[gi][ii],
                    m_vValue[gi][ii]);

            C1_up += m_pPrev[dim]->m_vHalf[gi][ii];
            C2_up += (m_vValue[gi][ii] + m_pPrev[dim]->m_vValue[gi][ii]) / 2;
        } else {
            C1_down += compute_exp(
                    vGases[gi]->getMass(),
                    m_vBoundaryTemperature[gi],
                    vImpulses[ii] - v3Speed * vGases[gi]->getMass());
        }
    }

    // Vacuum
    if (m_vBoundaryPressure[gi] == 0.0) {
        C1_up = 0.0;
        C2_up = 0.0;
    } else {
        C1_up = m_vBoundaryPressure[gi] / m_vBoundaryTemperature[gi] / pImpulse->getDeltaImpulseQube() - C1_up;
        C2_up = m_vBoundaryPressure[gi] / m_vBoundaryTemperature[gi] / pImpulse->getDeltaImpulseQube() - C2_up;

        if (C1_up < 0.0)
            C1_up = 0.0;
        if (C2_up < 0.0)
            C2_up = 0.0;
    }

    for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
        if (vImpulses[ii][dim] < 0) {
            m_pPrev[dim]->m_vHalf[gi][ii] = C1_up / C1_down * compute_exp(
                    vGases[gi]->getMass(),
                    m_vBoundaryTemperature[gi],
                    vImpulses[ii] - v3Speed * vGases[gi]->getMass());

            m_vValue[gi][ii] = 2 * C2_up / C1_down * compute_exp(
                    vGases[gi]->getMass(),
                    m_vBoundaryTemperature[gi],
                    vImpulses[ii] - v3Speed * vGases[gi]->getMass()) - m_pPrev[dim]->m_vValue[gi][ii];
            if (m_vValue[gi][ii] < 0.0)
                m_vValue[gi][ii] = 0.0;
        }
    }
}

/* Setting flow boundary type. */
void Cell::compute_half_flow_left(unsigned int dim, int gi) {
    const GasVector& vGases = m_pConfig->getGases();
    Impulse* pImpulse = m_pSolver->GetImpulse();
    ImpulseVector& vImpulses = pImpulse->getVector();

    double C1_up = 0.0;
    double C1_down = 0.0;
    double C2_up = 0.0;

    for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
        if (vImpulses[ii][dim] < 0) {
            double y = m_pConfig->getTimestep() / vGases[gi]->getMass() * std::abs(vImpulses[ii][dim] / m_vAreastep[dim]);

            m_vValue[gi][ii] = 2 * m_pNext[dim]->m_vValue[gi][ii] - m_pNext[dim]->m_pNext[dim]->m_vValue[gi][ii];
            if (m_vValue[gi][ii] < 0)
                m_vValue[gi][ii] = 0;

            m_vHalf[gi][ii] = m_pNext[dim]->m_vValue[gi][ii] - (1 - y) / 2 * limiter(
                    m_vValue[gi][ii],
                    m_pNext[dim]->m_vValue[gi][ii],
                    m_pNext[dim]->m_pNext[dim]->m_vValue[gi][ii]);

            C1_up += vImpulses[ii][dim] * m_vHalf[gi][ii];
            C2_up += vImpulses[ii][dim] * (m_vValue[gi][ii] + m_pNext[dim]->m_vValue[gi][ii]) / 2;
        } else {
            C1_down += vImpulses[ii][dim] * compute_exp(
                    vGases[gi]->getMass(),
                    m_vBoundaryTemperature[gi],
                    vImpulses[ii]);
        }
    }

    C1_up = m_vBoundaryStream[gi][dim] / pImpulse->getDeltaImpulseQube() - C1_up;
    C2_up = m_vBoundaryStream[gi][dim] / pImpulse->getDeltaImpulseQube() - C2_up;

    if (C1_up / C1_down < 0.0)
        C1_up = 0.0;
    if (C2_up / C1_down < 0.0)
        C2_up = 0.0;

    for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
        if (vImpulses[ii][dim] > 0) {
            m_vHalf[gi][ii] = C1_up / C1_down * compute_exp(
                    vGases[gi]->getMass(),
                    m_vBoundaryTemperature[gi],
                    vImpulses[ii]);

            m_vValue[gi][ii] = 2 * C2_up / C1_down * compute_exp(
                    vGases[gi]->getMass(),
                    m_vBoundaryTemperature[gi],
                    vImpulses[ii]) - m_pNext[dim]->m_vValue[gi][ii];
            if (m_vValue[gi][ii] < 0.0)
                m_vValue[gi][ii] = 0.0;
        }
    }
}

void Cell::compute_half_flow_right(unsigned int dim, int gi) {
    const GasVector& vGases = m_pConfig->getGases();
    Impulse* pImpulse = m_pSolver->GetImpulse();
    ImpulseVector& vImpulses = pImpulse->getVector();

    double C1_up = 0.0;
    double C1_down = 0.0;
    double C2_up = 0.0;

    for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
        if (vImpulses[ii][dim] > 0) {
            double y = m_pConfig->getTimestep() / vGases[gi]->getMass() * std::abs(vImpulses[ii][dim] / m_vAreastep[dim]);

            m_vValue[gi][ii] = 2 * m_pPrev[dim]->m_vValue[gi][ii] - m_pPrev[dim]->m_pPrev[dim]->m_vValue[gi][ii];
            if (m_vValue[gi][ii] < 0)
                m_vValue[gi][ii] = 0;

            m_pPrev[dim]->m_vHalf[gi][ii] = m_pPrev[dim]->m_vValue[gi][ii] + (1 - y) / 2 * limiter(
                    m_pPrev[dim]->m_pPrev[dim]->m_vValue[gi][ii],
                    m_pPrev[dim]->m_vValue[gi][ii],
                    m_vValue[gi][ii]);

            C1_up += std::abs(vImpulses[ii][dim]) * m_pPrev[dim]->m_vHalf[gi][ii];
            C2_up += std::abs(vImpulses[ii][dim]) * (m_vValue[gi][ii] + m_pPrev[dim]->m_vValue[gi][ii]) / 2;
        } else {
            C1_down += std::abs(vImpulses[ii][dim]) * compute_exp(
                    vGases[gi]->getMass(),
                    m_vBoundaryTemperature[gi],
                    vImpulses[ii]);
        }
    }

    C1_up = m_vBoundaryStream[gi][dim] / pImpulse->getDeltaImpulseQube() - C1_up;
    C2_up = m_vBoundaryStream[gi][dim] / pImpulse->getDeltaImpulseQube() - C2_up;

    if (C1_up / C1_down < 0.0)
        C1_up = 0.0;
    if (C2_up / C1_down < 0.0)
        C2_up = 0.0;

    for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
        if (vImpulses[ii][dim] < 0) {
            m_pPrev[dim]->m_vHalf[gi][ii] = C1_up / C1_down * compute_exp(
                    vGases[gi]->getMass(),
                    m_vBoundaryTemperature[gi],
                    vImpulses[ii]);

            m_vValue[gi][ii] = 2 * C2_up / C1_down * compute_exp(
                    vGases[gi]->getMass(),
                    m_vBoundaryTemperature[gi],
                    vImpulses[ii]) - m_pPrev[dim]->m_vValue[gi][ii];
            if (m_vValue[gi][ii] < 0.0)
                m_vValue[gi][ii] = 0.0;
        }
    }
}

/* Setting mirror boundary type. */
void Cell::compute_half_mirror_left(unsigned int dim, int gi) {
    const GasVector& vGases = m_pConfig->getGases();
    Impulse* pImpulse = m_pSolver->GetImpulse();
    ImpulseVector& vImpulses = pImpulse->getVector();

    for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
        double y = m_pConfig->getTimestep() / vGases[gi]->getMass() * std::abs(vImpulses[ii][dim] / m_vAreastep[dim]);

        if (vImpulses[ii][dim] > 0) {
            int ri = pImpulse->reverseIndex(ii, static_cast<sep::Axis>(dim));

            m_vHalf[gi][ii] = m_vValue[gi][ii] + (1 - y) / 2 * limiter(
                    m_vValue[gi][ri], // reversed for left
                    m_vValue[gi][ii],
                    m_pNext[dim]->m_vValue[gi][ii]);
        } else {
            m_vHalf[gi][ii] = m_pNext[dim]->m_vValue[gi][ii] - (1 - y) / 2 * limiter(
                    m_vValue[gi][ii],
                    m_pNext[dim]->m_vValue[gi][ii],
                    m_pNext[dim]->m_pNext[dim]->m_vValue[gi][ii]);
        }
    }

    for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
        double y = m_pConfig->getTimestep() / vGases[gi]->getMass() * vImpulses[ii][dim] / m_vAreastep[dim];
        int ri = pImpulse->reverseIndex(ii, static_cast<sep::Axis>(dim));
        m_vValue[gi][ii] = m_vValue[gi][ii] - y * (m_vHalf[gi][ii] - m_vHalf[gi][ri]);
    }
}

void Cell::compute_half_mirror_right(unsigned int dim, int gi) {
    const GasVector& vGases = m_pConfig->getGases();
    Impulse* pImpulse = m_pSolver->GetImpulse();
    ImpulseVector& vImpulses = pImpulse->getVector();

    for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
        double y = m_pConfig->getTimestep() / vGases[gi]->getMass() * std::abs(vImpulses[ii][dim] / m_vAreastep[dim]);

        int ri = pImpulse->reverseIndex(ii, static_cast<sep::Axis>(dim));

        if (vImpulses[ii][dim] > 0) {
            // PREV
            m_pPrev[dim]->m_vHalf[gi][ii] = m_pPrev[dim]->m_vValue[gi][ii] + (1 - y) / 2 * limiter(
                    m_pPrev[dim]->m_pPrev[dim]->m_vValue[gi][ii],
                    m_pPrev[dim]->m_vValue[gi][ii],
                    m_vValue[gi][ii]);

            // CURRENT
            m_vHalf[gi][ii] = m_vValue[gi][ii] + (1 - y) / 2 * limiter(
                    m_pPrev[dim]->m_vValue[gi][ii],
                    m_vValue[gi][ii],
                    m_vValue[gi][ri]); // reversed for right
        } else {
            // PREV
            m_pPrev[dim]->m_vHalf[gi][ii] = m_vValue[gi][ii] - (1 - y) / 2 * limiter(
                    m_pPrev[dim]->m_vValue[gi][ii],
                    m_vValue[gi][ii],
                    m_vValue[gi][ri]);

            // CURRENT
            m_vHalf[gi][ii] = m_vValue[gi][ri] - (1 - y) / 2 * limiter(
                    m_vValue[gi][ii],
                    m_vValue[gi][ri],
                    m_pPrev[dim]->m_vValue[gi][ii]);
        }
    }

    for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
        double y = m_pConfig->getTimestep() / vGases[gi]->getMass() * vImpulses[ii][dim] / m_vAreastep[dim];
        m_vValue[gi][ii] = m_vValue[gi][ii] - y * (m_vHalf[gi][ii] - m_pPrev[dim]->m_vHalf[gi][ii]);
    }
}

double Cell::compute_exp(const double& mass, const double& temp, const Vector3d& impulse) {
    return std::exp(-impulse.mod2() / mass / 2 / temp);
}

double Cell::limiter_superbee(const double& x, const double& y, const double& z) {
    if ((z - y) * (y - x) <= 0) {
        return 0.0;
    } else {
        return std::max(0.0, std::min(2 * std::abs(y - x), std::min(std::abs(z - y), std::min(std::abs(y - x), 2 * std::abs(z - y))))) * sgn(z - y);
    }
}

/* Macro Data */

double Cell::compute_concentration(unsigned int gi) {
    Impulse* pImpulse = m_pSolver->GetImpulse();
    ImpulseVector& vImpulses = pImpulse->getVector();

    double concentration = 0.0;
    for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
        concentration += m_vValue[gi][ii];
    }
    concentration *= pImpulse->getDeltaImpulseQube();
    return concentration;
}

double Cell::compute_temperature(unsigned int gi) {
    const GasVector& gasv = m_pConfig->getGases();
    Impulse* impulse = m_pSolver->GetImpulse();
    ImpulseVector& impulsev = impulse->getVector();

    double concentration = m_vMacroData[gi].dDensity;
    double temperature = 0.0;
    Vector3d vAverageSpeed = m_vMacroData[gi].Stream;
    vAverageSpeed /= concentration;

    for (unsigned int ii = 0; ii < impulsev.size(); ii++) {
        Vector3d tempvec;
        for (unsigned int vi = 0; vi < vAverageSpeed.getMass().size(); vi++) {
            tempvec[vi] = impulsev[ii][vi] / gasv[gi]->getMass() - vAverageSpeed[vi];
        }
        temperature += gasv[gi]->getMass() * tempvec.mod2() * m_vValue[gi][ii];
    }
    //cout << uvec[0] << ":" << uvec[1] << ":" << uvec[2] << endl;
    temperature *= impulse->getDeltaImpulseQube() / concentration / 3;

    //std::cout << temperature << std::endl;
    return temperature;
}

double Cell::compute_pressure(unsigned int gi) {
    return m_vMacroData[gi].dDensity * m_vMacroData[gi].dTemperature;
}

Vector3d Cell::compute_stream(unsigned int gi) {
    const GasVector& vGases = m_pConfig->getGases();
    Impulse* pImpulse = m_pSolver->GetImpulse();
    ImpulseVector& vImpulses = pImpulse->getVector();

    Vector3d vStream;
    for (unsigned int ii = 0; ii < vImpulses.size(); ii++) {
        for (unsigned int vi = 0; vi < vStream.getMass().size(); vi++) {
            vStream[vi] += vImpulses[ii][vi] * m_vValue[gi][ii];
        }
    }
    vStream *= pImpulse->getDeltaImpulseQube() / vGases[gi]->getMass();

    return vStream;
}

Vector3d Cell::compute_heatstream(unsigned int gi) {
    return Vector3d();
}
