#include "grid_constructor.h"

// This method being invoked while grid configuration
// Feel free to edit this method
void GridConstructor::ConfigureGridGeometry() {
  PushTemperature(1.0);
  PushConcentration(1.0);
  
  SetBox(Vector2i(1, 1), Vector2i(8, 8));

  PushTemperature(2.0);
  SetBox(Vector2i(3, 3), Vector2i(2, 2));
}