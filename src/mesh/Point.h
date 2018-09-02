//
// Created by Dmitry Sherbakov on 17.08.2018.
//

#ifndef TESTING_POINT_H
#define TESTING_POINT_H

#include "Element.h"

class Point : public Element {
public:
    Point(int id, const std::vector<Node*>& nodes) : Element(Type::POINT, id, nodes) {}
};

#endif //TESTING_POINT_H
