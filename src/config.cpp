#include "config.h"

using namespace std;

std::string Config::sName = "default";
Vector3i Config::vGridSize = Vector3i(0, 0, 1); // dynamic determination of the grid size
sep::GridGeometry Config::eGridGeometryType = sep::COMB_GRID_GEOMETRY;
double Config::dTimestep = 0.1;  // if needed may decrease automatically later
int Config::iMaxIteration = 100;
bool Config::bUseIntegral = true;
std::shared_ptr<HTypeGridConfig> Config::pHTypeGridConfig = std::shared_ptr<HTypeGridConfig>(new HTypeGridConfig);
std::string Config::sOutputPrefix = "../";
Vector2d Config::vCellSize = Vector2d(1.0, 1.0);  // default cell size
void Config::PrintMe() {
  cout << "Config " << sName << endl;
  cout << "GridSize = " << vGridSize.x() <<
  "x" << vGridSize.y() <<
  "x" << vGridSize.z() << endl;
}
