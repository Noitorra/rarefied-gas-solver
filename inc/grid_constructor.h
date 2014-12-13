#ifndef GRID_CONSTRUCTOR_H_
#define GRID_CONSTRUCTOR_H_

#include "main.h"
#include <functional>

// GridManager interface
class GridConstructor {
protected:
    void ConfigureGridGeometry();
  
private:
  virtual void PushTemperature(double dT) = 0;
  virtual double PopTemperature() = 0;
  virtual void PushConcentration(double dConc) = 0;
  virtual double PopConcentration() = 0;
  virtual void PrintGrid() = 0;
  // Set box to normal
  virtual void SetBox(Vector2i vP, Vector2i vSize,
          std::function<void(int x, int y, CellConfig* config)> config_func =
          [] (int x, int y, CellConfig* config) {}) = 0;
};

#endif // GRID_CONSTRUCTOR_H_