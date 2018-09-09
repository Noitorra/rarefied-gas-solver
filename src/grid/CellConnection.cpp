#include "CellConnection.h"
#include "NormalCell.h"

CellConnection::CellConnection(BaseCell* first, BaseCell* second, double square, const Vector3d& normal12)
        : _first(first), _second(second), _square(square), _normal12(normal12), _normal21(-normal12) {}

BaseCell* CellConnection::getFirst() const {
    return _first;
}

BaseCell* CellConnection::getSecond() const {
    return _second;
}

double CellConnection::getSquare() const {
    return _square;
}

const Vector3d& CellConnection::getNormal12() const {
    return _normal12;
}

const Vector3d& CellConnection::getNormal21() const {
    return _normal21;
}

const double CellConnection::getValue(unsigned int gi, unsigned int ii, Vector3d impulse) const {
    double projection = _normal12.scalar(impulse);
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
