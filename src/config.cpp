#include "config.h"
#include "parameters/gas.h"
#include "parameters/beta_chain.h"
#include "normalizer.h"

using namespace std;

double Config::dTimestep = 0.0; // sets automaticaly in grid_manager.cpp
int Config::iMaxIteration = 2000;
std::string Config::sOutputPrefix = "../";
bool Config::bUseIntegral = true;
bool Config::bUseBetaChain = false;
int Config::iOutEach = 100;

// Grid Related
Vector3i Config::vGridSize = Vector3i(0, 0, 1); // dynamic determination of the grid size
HTypeGridConfig* Config::pHTypeGridConfig = new HTypeGridConfig;
Vector2d Config::vCellSize = Vector2d(1.0, 1.0); // default cell size in mm!
bool Config::bGPRTGrid = false;

// Gases Related
GasVector Config::vGas;
int Config::iGasesNumber = 2;
BetaChainVector Config::vBetaChains;
int Config::iBetaChainsNumber = 0;

Normalizer* Config::pNormalizer = new Normalizer();


void Config::Init() {
	// Fill gases. Use only iNumGases first gases.
	vGas.push_back(new Gas(1.0)); //133 Cs
	vGas.push_back(new Gas(88.0 / 133.0)); //88 Kr -> Rb -> Sr
	vGas.push_back(new Gas(138.0 / 133.0)); //138 Xe -> Cs -> Ba
	vGas.push_back(new Gas(88.0 / 133.0)); //88 Rb
	vGas.push_back(new Gas(88.0 / 133.0)); //88 Sr
	vGas.push_back(new Gas(138.0 / 133.0)); //138 Cs
	vGas.push_back(new Gas(138.0 / 133.0)); //138 Ba

	// Fill beta chains, use only iBetaChains first.
	//vBetaChains.push_back(std::make_shared<BetaChain>(1, 2, 3, 6.78e-5, 6.49e-4)); // test!!!

	vBetaChains.push_back(new BetaChain(1, 3, 4, 6.78e-5, 6.49e-4));
	vBetaChains.push_back(new BetaChain(2, 5, 6, 6.78e-5, 6.49e-4));

	pNormalizer->loadFromFile();
}

void Config::PrintMe() {
	cout << "GridSize = " << vGridSize.x() <<
			"x" << vGridSize.y() <<
			"x" << vGridSize.z() << endl;
}
