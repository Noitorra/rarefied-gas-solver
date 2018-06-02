#ifndef RGS_GRIDCONSTRUCTOR_H
#define RGS_GRIDCONSTRUCTOR_H

#include "GridBox.h"

class GridConstructor {
public:
    virtual std::vector<GridBox*> createBoxes() = 0;
};

#endif //RGS_GRIDCONSTRUCTOR_H
