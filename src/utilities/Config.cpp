#include <sstream>
#include "Config.h"
#include "parameters/Gas.h"
#include "parameters/BetaChain.h"
#include "parameters/Impulse.h"
#include "Normalizer.h"

Config::Config() : m_pNormalizer(new Normalizer()), m_pImpulse(new Impulse()) {}

void Config::init() {
    m_sOutputFolder = "c:/Dev/Projects/!MAIN/output/";

    // Grid Related
    m_vGridSize = Vector3u(22, 12, 1);
    m_vCellSize = Vector2d(1.0, 1.0); // default cell size in mm!

    m_iMaxIteration = 100;
    m_iOutEach = 1;

    m_bUseIntegral = false;
    m_bUseBetaChain = false;

    m_vAxis.push_back(Axis::X);
    m_vAxis.push_back(Axis::Y);

    m_dTimestep = 0.01; // TODO: set it during grid creation

    // Gases related
    m_iGasesCount = 1;
    m_vGases.emplace_back(1.0);           // 133 Cs
    m_vGases.emplace_back(88.0 / 133.0);  // 88 Kr -> Rb -> Sr
    m_vGases.emplace_back(138.0 / 133.0); // 138 Xe -> Cs -> Ba
    m_vGases.emplace_back(88.0 / 133.0);  // 88 Rb
    m_vGases.emplace_back(88.0 / 133.0);  // 88 Sr
    m_vGases.emplace_back(138.0 / 133.0); // 138 Cs
    m_vGases.emplace_back(138.0 / 133.0); // 138 Ba

    // Beta chains related
    m_iBetaChainsCount = 0;
    m_vBetaChains.emplace_back(1, 3, 4, 6.78e-5, 6.49e-4);
    m_vBetaChains.emplace_back(2, 5, 6, 6.78e-5, 6.49e-4);

    // Normalization
    m_pNormalizer->init();

    // Impulse
    m_pImpulse->Init();
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
