#ifndef CONFIG_H_
#define CONFIG_H_

#include "main.h"

class Gas;
class BetaChain;
class Normalizer;

typedef std::vector<Gas*> GasVector;
typedef std::vector<BetaChain*> BetaChainVector;

struct HTypeGridConfig {
	int D;
	int l;
	int d;
	int h;
	int gaps_q;
	double T1, T2;
	double n1, n2, n3, n4;
};

struct Config {

public:
    static Config* getInstance() {
        static Config* pConfig = new Config();
        return pConfig;
    }

    Config();
    void Init();
    void PrintMe();

    Vector3i getGridSize() const {
        return m_vGridSize;
    }

    HTypeGridConfig* getHTypeGridConfig() const {
        return m_pHTypeGridConfig;
    }

    bool isGPRTGrid() const {
        return m_bGPRTGrid;
    }

    void setCellSize(const Vector2d& vCellSize) {
        m_vCellSize = vCellSize;
    }

    Vector2d getCellSize() const {
        return m_vCellSize;
    }

    GasVector getGases() const {
        return m_vGases;
    }

    int getGasesNum() const {
        return m_iGasesNum;
    }

    BetaChainVector getBetaChains() const {
        return m_vBetaChains;
    }

    int getBetaChainsNum() const {
        return m_iBetaChainsNum;
    }

    double getTimestep() const {
        return m_dTimestep;
    }

    int getMaxIteration() const {
        return m_iMaxIteration;
    }

    bool isUseIntegral() const {
        return m_bUseIntegral;
    }

    std::string getOutputPrefix() const {
        return m_sOutputPrefix;
    }

    bool isUseBetaChains() const {
        return m_bUseBetaChain;
    }

    int getOutEach() const {
        return m_iOutEach;
    }

    Normalizer* getNormalizer() const {
        return m_pNormalizer;
    }
private:

	// Grid Related
	Vector3i m_vGridSize;
	HTypeGridConfig* m_pHTypeGridConfig;
	bool m_bGPRTGrid;
	Vector2d m_vCellSize; // default cell size in mm

	// Gas Related
	GasVector m_vGases;
	int m_iGasesNum;
	BetaChainVector m_vBetaChains;
	int m_iBetaChainsNum;

	double m_dTimestep;
	int m_iMaxIteration;
	bool m_bUseIntegral;
	std::string m_sOutputPrefix;
	bool m_bUseBetaChain;
	int m_iOutEach;

	Normalizer* m_pNormalizer;
};

#endif // CONFIG_H_
