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
    CellConnection(BaseCell* first, BaseCell* second, double square, const Vector3d& normal12);

    BaseCell* getFirst() const;

    BaseCell* getSecond() const;

    double getSquare() const;

    const Vector3d& getNormal12() const;

    const Vector3d& getNormal21() const;

    const double getValue(unsigned int gi, unsigned int ii, Vector3d impulse) const;

};


#endif //RGS_CELLCONNECTION_H
