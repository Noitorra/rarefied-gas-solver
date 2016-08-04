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
    Config* getInstance() {
        static Config* pConfig = new Config();
        return pConfig;
    }

    Config();
    void Init();
    void PrintMe();

    Vector3i getGridSize() {
        return m_vGridSize;
    }

    HTypeGridConfig* getHTypeGridConfig() {
        return m_pHTypeGridConfig;
    }

    bool isGPRTGrid() {
        return m_bGPRTGrid;
    }

    Vector2d getCellSize() {
        return m_vCellSize;
    }

    GasVector getGases() {
        return m_vGases;
    }

    int getGasesNum() {
        return m_iGasesNum;
    }

    BetaChainVector getBetaChains() {
        return m_vBetaChains;
    }

    int getBetaChainsNum() {
        return m_iBetaChainsNum;
    }

    double getTimestep() {
        return m_dTimestep;
    }

    int getMaxIteration() {
        return m_iMaxIteration;
    }

    bool isUseIntegral() {
        return m_bUseIntegral;
    }

    std::string getOutputPrefix() {
        return m_sOutputPrefix;
    }

    bool isUseBetaChains () {
        return m_bUseBetaChain;
    }

    int getOutEach() {
        return m_iOutEach;
    }

    Normalizer* getNormalizer() {
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
