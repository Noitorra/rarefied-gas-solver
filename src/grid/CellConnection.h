#ifndef RGS_CELLCONNECTION_H
#define RGS_CELLCONNECTION_H

#include <utilities/Types.h>

class Cell;

class CellConnection {
private:
    Cell* _first;
    Cell* _second;

    double _square;
    Vector3d _normal12;
    Vector3d _normal21;

public:
    CellConnection(Cell* first, Cell* second, double square, const Vector3d& normal12);

    Cell* getFirst() const;

    Cell* getSecond() const;

    double getSquare() const;

    const Vector3d& getNormal12() const;

    const Vector3d& getNormal21() const;

    const double getValue(unsigned int gi, unsigned int ii, Vector3d impulse) const;

};


#endif //RGS_CELLCONNECTION_H
