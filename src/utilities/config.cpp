#include "config.h"
#include "parameters/gas.h"
#include "parameters/beta_chain.h"
#include "parameters/impulse.h"
#include "normalizer.h"

Config::Config() : m_pNormalizer(new Normalizer()), m_pImpulse(new Impulse()) {}

void Config::init() {
    m_dTimestep = 0.0; // sets automatically in grid_manager.cpp
    m_iMaxIteration = 2000;
    m_sOutputPrefix = "../";
    m_bUseIntegral = true;
    m_bUseBetaChain = false;
    m_iOutEach = 100;

    // Grid Related
    m_vGridSize = Vector3i(0, 0, 1); // dynamic determination of the grid size
    m_pHTypeGridConfig = new HTypeGridConfig;
    m_vCellSize = Vector2d(1.0, 1.0); // default cell size in mm!
    m_bGPRTGrid = false;

    // Gases Related
    m_iGasesNum = 2;
    m_iBetaChainsNum = 0;

    // Fill gases. Use only iNumGases first gases.
    m_vGases.clear();
    m_vGases.push_back(new Gas(1.0)); // 133 Cs
    m_vGases.push_back(new Gas(88.0 / 133.0)); // 88 Kr -> Rb -> Sr
    m_vGases.push_back(new Gas(138.0 / 133.0)); // 138 Xe -> Cs -> Ba
    m_vGases.push_back(new Gas(88.0 / 133.0)); // 88 Rb
    m_vGases.push_back(new Gas(88.0 / 133.0)); // 88 Sr
    m_vGases.push_back(new Gas(138.0 / 133.0)); // 138 Cs
    m_vGases.push_back(new Gas(138.0 / 133.0)); // 138 Ba

    // Fill beta chains, use only iBetaChains first.
    // m_vBetaChains.push_back(std::make_shared<BetaChain>(1, 2, 3, 6.78e-5, 6.49e-4)); // test!!!
    m_vBetaChains.clear();
    m_vBetaChains.push_back(new BetaChain(1, 3, 4, 6.78e-5, 6.49e-4));
    m_vBetaChains.push_back(new BetaChain(2, 5, 6, 6.78e-5, 6.49e-4));

    // Normalization
    m_pNormalizer->init();

    // Impulse
    m_pImpulse->Init();
}

std::string Config::toString() const {
    std::ostringstream os;
    os << "GridSize = [" << m_vGridSize.x() << "x" << m_vGridSize.y() << "x" << m_vGridSize.z() << "]";
    return os.str();
}

Impulse* Config::getImpulse() const {
    return m_pImpulse;
}
