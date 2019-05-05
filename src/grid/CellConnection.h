#ifndef RGS_CELLCONNECTION_H
#define RGS_CELLCONNECTION_H

#include <utilities/Types.h>

class BaseCell;

class CellConnection {
private:
    BaseCell* _first;
    BaseCell* _second;

    double _square;
    Vector3d _normal12;
    Vector3d _normal21;

public:
    CellConnection(BaseCell* first, BaseCell* second, double square, const Vector3d& normal12)
    : _first(first), _second(second), _square(square), _normal12(normal12), _normal21(-normal12) {}

    BaseCell* getFirst() const {
        return _first;
    }

    BaseCell* getSecond() const {
        return _second;
    }

    double getSquare() const {
        return _square;
    }

    const Vector3d& getNormal12() const {
        return _normal12;
    }

    const Vector3d& getNormal21() const {
        return _normal21;
    }

};


#endif //RGS_CELLCONNECTION_H
