#ifndef RGS_GPRTCONSTRUCTOR_H
#define RGS_GPRTCONSTRUCTOR_H

#include "GridConstructor.h"
#include "CellData.h"

class GPRTConstructor : public GridConstructor {
public:
    std::vector<GridBox*> createBoxes() override {
        std::vector<GridBox*> boxes;

        GridBox* box = nullptr;

        const double sizeX = 5.0 / 18;
        const double sizeY = 0.4 / 5;

        // creation of structure

        return boxes;
    }
};

#endif //RGS_GPRTCONSTRUCTOR_H
