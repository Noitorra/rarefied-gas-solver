#include "grid_constructor.h"

// This method being invoked while grid configuration
// Feel free to edit this method
void GridConstructor::ConfigureGridGeometry() {
  PushTemperature(1.0);
  PushConcentration(1.0);
  
  SetBox(Vector2i(0, 0), Vector2i(25, 25));

  PushTemperature(2.0);
  SetBox(Vector2i(10, 10), Vector2i(5, 5));
}