#ifndef TESTING_TRIANGLE_H
#define TESTING_TRIANGLE_H

#include "Element.h"
#include "Line.h"

class Triangle : public Element {
public:
    Triangle(int id, const std::vector<Node*>& nodes) : Element(Type::LINE, id, nodes) {
        Vector3d v1 = _nodes[1]->getPosition() - _nodes[0]->getPosition();
        Vector3d v2 = _nodes[2]->getPosition() - _nodes[0]->getPosition();
        Vector3d v3 = _nodes[2]->getPosition() - _nodes[1]->getPosition();

        _sideElements.emplace_back(new Line(-1, {_nodes[0], _nodes[1]}));
        _sideElements.back()->setNormal(-v1.vector(v2).vector(v1).normalize());

        _sideElements.emplace_back(new Line(-1, {_nodes[1], _nodes[2]}));
        _sideElements.back()->setNormal(v3.vector(v1).vector(v3).normalize());

        _sideElements.emplace_back(new Line(-1, {_nodes[2], _nodes[0]}));
        _sideElements.back()->setNormal(v2.vector(v3).vector(v2).normalize());

        _volume = v1.vector(v2).module() / 2;
    }
};


#endif //TESTING_TRIANGLE_H
