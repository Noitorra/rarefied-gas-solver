#ifndef CONFIG_H
#define CONFIG_H

#include "Types.h"

#include <vector>
#include <string>
#include <ostream>

class Gas;
class BetaChain;
class Normalizer;
class Impulse;

struct Config {
private:
    std::string m_sOutputFolder;

    bool m_bUseIntegral;
    bool m_bUseBetaChain;

    // Gas related
    std::vector<Gas> m_vGases;

    // Beta chains
    std::vector<BetaChain> m_vBetaChains;

    double m_dTimestep;

    unsigned int m_iMaxIteration;
    unsigned int m_iOutEach;

    Normalizer* m_pNormalizer;
    Impulse* m_pImpulse;

public:
    static Config* getInstance() {
        static auto* pConfig = new Config();
        return pConfig;
    }

    Config();

    void init();

    const std::vector<Gas>& getGases() const {
        return m_vGases;
    }

    const std::vector<BetaChain>& getBetaChains() const {
        return m_vBetaChains;
    }

    void setTimestep(double dTimestep) {
        m_dTimestep = dTimestep;
    }

    double getTimestep() const {
        return m_dTimestep;
    }

    unsigned int getMaxIteration() const {
        return m_iMaxIteration;
    }

    bool isUseIntegral() const {
        return m_bUseIntegral;
    }

    const std::string& getOutputFolder() const {
        return m_sOutputFolder;
    }

    bool isUseBetaChains() const {
        return m_bUseBetaChain;
    }

    unsigned int getOutEach() const {
        return m_iOutEach;
    }

    Normalizer* getNormalizer() const {
        return m_pNormalizer;
    }

    Impulse* getImpulse() const {
        return m_pImpulse;
    }

    friend std::ostream& operator<<(std::ostream& os, const Config& config);
};

#endif // CONFIG_H
