#include "config.h"
#include "parameters/gas.h"

using namespace std;

std::string Config::sName = "default";
Vector3i Config::vGridSize = Vector3i(0, 0, 1); // dynamic determination of the grid size
sep::GridGeometry Config::eGridGeometryType = sep::COMB_GRID_GEOMETRY;
double Config::dTimestep = 0.1;  // if needed may decrease automatically later
int Config::iMaxIteration = 2000;
bool Config::bUseIntegral = true;
std::shared_ptr<HTypeGridConfig> Config::pHTypeGridConfig = std::shared_ptr<HTypeGridConfig>(new HTypeGridConfig);
std::string Config::sOutputPrefix = "../";
Vector2d Config::vCellSize = Vector2d(1.0, 1.0);  // default cell size
bool Config::bGPRTGrid = false;
int Config::iOutEach = 1;
Vector2d Config::vCellSize = Vector2d(1.0, 1.0);  // default cell size in mm!

double Config::n_normalize = 1.0; // not assigned
double Config::T_normalize = 1.0; // not assigned
double Config::P_normalize = 1.0; // not assigned
double Config::tau_normalize = 1.0; // not assigned
double Config::m_normalize = 1.0; // not assigned
double Config::e_cut_normalize = 1.0; // not assigned
double Config::l_normalize = 1.0; // not assigned

int Config::iGasesNumber = 2;
GasVector Config::vGas;


void Config::Init()
{
  // Handle Gas Info ...
  vGas.resize(iGasesNumber);
  if (iGasesNumber == 1)
  {
    vGas[0] = std::shared_ptr<Gas>(new Gas(1.0));
  }
  else if (iGasesNumber == 2)
  {
    vGas[0] = std::shared_ptr<Gas>(new Gas(1.0));
    vGas[1] = std::shared_ptr<Gas>(new Gas(1.0));
  }
  else if (iGasesNumber == 3)
  {
    vGas[0] = std::shared_ptr<Gas>(new Gas(1.0));
    vGas[1] = std::shared_ptr<Gas>(new Gas(0.5));
    vGas[2] = std::shared_ptr<Gas>(new Gas(0.5));
  } else
  {
    for (int i = 0; i < iGasesNumber; i++)
    {
      vGas[i] = std::shared_ptr<Gas>(new Gas(1.0));
    }
  }
}

void Config::PrintMe() {
  cout << "Config " << sName << endl;
  cout << "GridSize = " << vGridSize.x() <<
  "x" << vGridSize.y() <<
  "x" << vGridSize.z() << endl;
}
