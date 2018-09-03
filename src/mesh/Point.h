#ifndef RGS_POINT_H
#define RGS_POINT_H

#include "Element.h"

class Point : public Element {
public:
    Point(int id, const std::vector<Node*>& nodes) : Element(Type::POINT, id, nodes) {}
};

#endif //RGS_POINT_H
