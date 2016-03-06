#include "config.h"
#include "parameters/gas.h"
#include "parameters/beta_chain.h"

using namespace std;

std::string Config::sName = "default";
double Config::dTimestep = 0.0;  // sets automaticaly in grid_manager.cpp
int Config::iMaxIteration = 2000;
std::string Config::sOutputPrefix = "../";
bool Config::bUseIntegral = false;
bool Config::bUseBetaChain = false;
int Config::iOutEach = 100;

// Grid Related
Vector3i Config::vGridSize = Vector3i(0, 0, 1); // dynamic determination of the grid size
sep::GridGeometry Config::eGridGeometryType = sep::COMB_GRID_GEOMETRY;
std::shared_ptr<HTypeGridConfig> Config::pHTypeGridConfig = std::shared_ptr<HTypeGridConfig>(new HTypeGridConfig);
Vector2d Config::vCellSize = Vector2d(1.0, 1.0);  // default cell size in mm!
bool Config::bGPRTGrid = false;

// Gases Related
GasVector Config::vGas;
int Config::iGasesNumber = 2;
BetaChainVector Config::vBetaChains;
int Config::iBetaChainsNumber = 0;

double Config::n_normalize = 1.0; // not assigned
double Config::T_normalize = 1.0; // not assigned
double Config::P_normalize = 1.0; // not assigned
double Config::tau_normalize = 1.0; // not assigned
double Config::m_normalize = 1.0; // not assigned
double Config::e_cut_normalize = 1.0; // not assigned
double Config::l_normalize = 1.0; // not assigned



void Config::Init() {
  // Fill gases. Use only iNumGases first gases.
  vGas.push_back(std::make_shared<Gas>(1.0)); //133 Cs
  vGas.push_back(std::make_shared<Gas>(88.0 / 133.0)); //88 Kr -> Rb -> Sr
  vGas.push_back(std::make_shared<Gas>(138.0 / 133.0)); //138 Xe -> Cs -> Ba
  vGas.push_back(std::make_shared<Gas>(88.0 / 133.0)); //88 Rb
  vGas.push_back(std::make_shared<Gas>(88.0 / 133.0)); //88 Sr
  vGas.push_back(std::make_shared<Gas>(138.0 / 133.0)); //138 Cs
  vGas.push_back(std::make_shared<Gas>(138.0 / 133.0)); //138 Ba

  // Fill beta chains, use only iBetaChains first.
  //vBetaChains.push_back(std::make_shared<BetaChain>(1, 2, 3, 6.78e-5, 6.49e-4)); // test!!!

  vBetaChains.push_back(std::make_shared<BetaChain>(1, 3, 4, 6.78e-5, 6.49e-4));
  vBetaChains.push_back(std::make_shared<BetaChain>(2, 5, 6, 6.78e-5, 6.49e-4));
}

void Config::PrintMe() {
  cout << "Config " << sName << endl;
  cout << "GridSize = " << vGridSize.x() <<
    "x" << vGridSize.y() <<
    "x" << vGridSize.z() << endl;
}
