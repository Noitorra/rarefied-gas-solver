#include "grid_constructor.h"

// This method being invoked while grid configuration
// Feel free to edit this method
void GridConstructor::ConfigureGridGeometry() {
  PushTemperature(1.5);
  PushConcentration(1.0);
  
  SetBox(Vector2i(0, 0), Vector2i(25, 25), [] (int x, int y, CellConfig* config) {
      config->wall_T = (x == 0 || x == 24) ? 2.0 : 1.0;
  });
}