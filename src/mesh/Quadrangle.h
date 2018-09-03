#ifndef RGS_QUADRANGLE_H
#define RGS_QUADRANGLE_H

#include "Element.h"
#include "Line.h"

class Quadrangle : public Element {
private:
    std::vector<std::shared_ptr<Line>> _diagonalElements;

public:
    Quadrangle(int id, const std::vector<Node*>& nodes) : Element(Type::QUADRANGLE, id, nodes) {

        // find all sides and diagonals
        for (unsigned int i = 0; i < _nodes.size(); i++) {
            for (unsigned int j = i; j < _nodes.size(); j++) {
                Line* line = new Line(-1, {_nodes[i], _nodes[j]});

                bool isSide = true;
                Vector3d* direction = nullptr;
                for (unsigned int k = 0; k < _nodes.size(); k++) {
                    if (k != i && k != j) {
                        Line* kLine = new Line(-1, {_nodes[i], _nodes[j]});
                        Vector3d kDirection = line->getVector().vector(kLine->getVector());
                        if (direction == nullptr) {
                            direction = new Vector3d(kDirection);
                        } else {
                            if (direction->scalar(kDirection) < 0) {
                                isSide = false;
                                break;
                            }
                        }
                    }
                }

                if (isSide == true) {
                    _sideElements.emplace_back(line);

                    Vector3d v1 = _nodes[j]->getPosition() - _nodes[i]->getPosition();
                    int p = 0;
                    for (unsigned int k = 0; k < _nodes.size(); k++) {
                        if (k != i && k != j) {
                            p = k;
                            break;
                        }
                    }
                    Vector3d v2 = _nodes[p]->getPosition() - _nodes[i]->getPosition();
                    _sideElements.back()->setNormal(-v1.vector(v2).vector(v1).normalize());
                } else {
                    _diagonalElements.emplace_back(line);
                }
            }
        }

        // volume is easy: d1d2sin(1,2) / 2
        Vector3d diag1 = _diagonalElements[0].get()->getVector();
        Vector3d diag2 = _diagonalElements[1].get()->getVector();
        _volume = diag1.vector(diag2).module() / 2;
    }
};


#endif //RGS_QUADRANGLE_H
