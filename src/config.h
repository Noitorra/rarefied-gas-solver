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
	
	// Grid Related
	static Vector3i vGridSize;
	static HTypeGridConfig* pHTypeGridConfig;
	static bool bGPRTGrid;
	static Vector2d vCellSize; // default cell size in mm

	// Gas Related
	static GasVector vGas;
	static int iGasesNumber;
	static BetaChainVector vBetaChains;
	static int iBetaChainsNumber;

	static double dTimestep;
	static int iMaxIteration;
	static bool bUseIntegral;
	static std::string sOutputPrefix;
	static bool bUseBetaChain;
	static int iOutEach;

	static Normalizer* pNormalizer;

	static void Init();
	static void PrintMe();
};

#endif // CONFIG_H_
