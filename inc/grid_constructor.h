#ifndef GRID_CONSTRUCTOR_H_
#define GRID_CONSTRUCTOR_H_

#include "main.h"

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
  virtual void SetBox(Vector2i vP, Vector2i vSize) = 0;
};

#endif // GRID_CONSTRUCTOR_H_