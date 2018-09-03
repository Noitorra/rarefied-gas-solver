#ifndef RGS_LINE_H
#define RGS_LINE_H

#include <cmath>
#include "Element.h"
#include "Point.h"

class Line : public Element {
private:
    Vector3d _vector;

public:
    Line(int id, const std::vector<Node*>& nodes) : Element(Type::LINE, id, nodes) {
        _vector = _nodes[1]->getPosition() - _nodes[0]->getPosition();

        _sideElements.emplace_back(new Point(-1, {_nodes[0]}));
        _sideElements.emplace_back(new Point(-1, {_nodes[1]}));

        _volume = _vector.module();
    }

    const Vector3d& getVector() const {
        return _vector;
    }
};


#endif //RGS_LINE_H
