#ifndef CONFIG_H_
#define CONFIG_H_

#include <vector>
#include <string>
#include <ostream>
#include "core/Application.h"
#include "Types.h"

class Gas;
class BetaChain;
class Normalizer;
class Impulse;

struct Config {
private:

    // Grid Related
    Vector3u m_vGridSize;
    Vector2d m_vCellSize; // default cell size in mm

    // Gas Related
    std::vector<Gas> m_vGases;
    unsigned int m_iGasesCount;
    std::vector<BetaChain> m_vBetaChains;
    unsigned int m_iBetaChainsCount;

    double m_dTimestep;
    bool m_bUseIntegral;
    bool m_bUseBetaChain;

    std::string m_sOutputFolder;
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

    void setGridSize(const Vector3u& vGridSize) {
        m_vGridSize = vGridSize;
    }

    const Vector3u& getGridSize() const {
        return m_vGridSize;
    }

    void setCellSize(const Vector2d& vCellSize) {
        m_vCellSize = vCellSize;
    }

    const Vector2d& getCellSize() const {
        return m_vCellSize;
    }

    const std::vector<Gas>& getGases() const {
        return m_vGases;
    }

    unsigned int getGasesCount() const {
        return m_iGasesCount;
    }

    const std::vector<BetaChain>& getBetaChains() const {
        return m_vBetaChains;
    }

    unsigned int getBetaChainsCount() const {
        return m_iBetaChainsCount;
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

    Impulse* getImpulse() const;

    friend std::ostream& operator<<(std::ostream& os, const Config& config);
};

#endif // CONFIG_H_
