#include <sstream>
#include "Config.h"
#include "parameters/Gas.h"
#include "parameters/BetaChain.h"
#include "parameters/Impulse.h"
#include "Normalizer.h"

Config::Config() : m_pNormalizer(new Normalizer()), m_pImpulse(new Impulse()) {}

void Config::init() {
    m_sOutputFolder = "../";

    m_dTimestep = 0.01; // TODO: set it during grid creation
    m_iMaxIteration = 1;
    m_iOutEach = 1;
    m_bUseIntegral = false;
    m_bUseBetaChain = false;

    // Grid Related
    m_vGridSize = Vector3u(42, 22, 1);
    m_vCellSize = Vector2d(1.0, 1.0); // default cell size in mm!

    // Gases Related
    m_iGasesCount = 1;
    m_iBetaChainsCount = 0;

    // Fill gases. Use only iNumGases first gases.
    m_vGases.clear();
    m_vGases.emplace_back(1.0); // 133 Cs
    m_vGases.emplace_back(88.0 / 133.0); // 88 Kr -> Rb -> Sr
    m_vGases.emplace_back(138.0 / 133.0); // 138 Xe -> Cs -> Ba
    m_vGases.emplace_back(88.0 / 133.0); // 88 Rb
    m_vGases.emplace_back(88.0 / 133.0); // 88 Sr
    m_vGases.emplace_back(138.0 / 133.0); // 138 Cs
    m_vGases.emplace_back(138.0 / 133.0); // 138 Ba

    // Fill beta chains, use only iBetaChains first.
    // m_vBetaChains.push_back(std::make_shared<BetaChain>(1, 2, 3, 6.78e-5, 6.49e-4)); // test!!!
    m_vBetaChains.clear();
    m_vBetaChains.emplace_back(1, 3, 4, 6.78e-5, 6.49e-4);
    m_vBetaChains.emplace_back(2, 5, 6, 6.78e-5, 6.49e-4);

    // Normalization
    m_pNormalizer->init();

    // Impulse
    m_pImpulse->Init();
}

Impulse* Config::getImpulse() const {
    return m_pImpulse;
}

std::ostream& operator<<(std::ostream& os, const Config& config) {
    os << "GridSize: " << config.m_vGridSize << std::endl
       << "CellSize: " << config.m_vCellSize << std::endl;

    os << "GasesCount: " << config.m_iGasesCount << std::endl;
    os << "Gases: [";
    for (unsigned int i = 0; i < config.m_iGasesCount; i++) {
        os << config.m_vGases[i];
        if (i != config.m_iGasesCount - 1) {
            os << ", ";
        }
    }
    os << "]" << std::endl;

    os << "BetaChainsCount: " << config.m_iBetaChainsCount << std::endl;
    os << "BetaChains: [";
    for (unsigned int i = 0; i < config.m_iBetaChainsCount; i++) {
        os << config.m_vBetaChains[i];
        if (i != config.m_iBetaChainsCount - 1) {
            os << ", ";
        }
    }
    os << "]" << std::endl;

    os << "Timestep: "     << config.m_dTimestep     << std::endl
       << "UseIntegral: "  << config.m_bUseIntegral  << std::endl
       << "UseBetaChain: " << config.m_bUseBetaChain << std::endl
       << "OutputPrefix: " << config.m_sOutputFolder << std::endl
       << "MaxIteration: " << config.m_iMaxIteration << std::endl
       << "OutEach: "      << config.m_iOutEach      << std::endl
       << "Normalizer: "   << config.m_pNormalizer   << std::endl
       << "Impulse: "      << config.m_pImpulse      << std::endl;
    return os;
}