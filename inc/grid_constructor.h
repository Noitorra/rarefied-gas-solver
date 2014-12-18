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
  virtual void PushPressure(double pressure) = 0;
  virtual double PopPressure() = 0;
  virtual void PrintGrid() = 0;
  // Set box to normal
  virtual void SetBox(Vector2d vP, Vector2d vSize,
          ConfigFunction config_func = [] (int x, int y, GasesConfigsMap& configs, struct GridBox* box) {}) = 0;
};

#endif // GRID_CONSTRUCTOR_H_