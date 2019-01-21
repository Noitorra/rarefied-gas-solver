#include "CellConnection.h"
#include "NormalCell.h"

double CellConnection::getValue(unsigned int gi, unsigned int ii, Vector3d impulse) const {
    double projection = _normal21.scalar(impulse);
    if (projection != 0) {
        double value;
        if (projection < 0) {
            value = _first->getValues()[gi][ii];
        } else {
            value = _second->getValues()[gi][ii];
        }
        return value * projection * _square;
    } else {
        return 0.0;
    }
}
