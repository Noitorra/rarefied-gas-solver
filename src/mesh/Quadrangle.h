#ifndef RGS_QUADRANGLE_H
#define RGS_QUADRANGLE_H

#include "Element.h"
#include "Line.h"

class Quadrangle : public Element {
    friend class boost::serialization::access;

private:
    std::vector<std::shared_ptr<Line>> _diagonalElements;

public:
    Quadrangle() = default;

    Quadrangle(int id, int physicalEntityId, int geomUnitId, const std::vector<int>& partitions, const std::vector<int>& nodeIds)
    : Element(Type::QUADRANGLE, id, physicalEntityId, geomUnitId, partitions, nodeIds) {}

    explicit Quadrangle(const std::vector<int>& nodeIds) : Quadrangle(0, -1, -1, {}, nodeIds) {}

private:
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & boost::serialization::base_object<Element>(*this);
        ar & _diagonalElements;
    }

    void innerInit(const std::vector<Node*>& nodes, bool isSideElementsRequired) override {

        // find all sides and diagonals
        _sideElements.clear();
        _diagonalElements.clear();
        for (auto i = 0; i < nodes.size(); i++) {
            for (auto j = i + 1; j < nodes.size(); j++) {
                Line* line = new Line({nodes[i]->getId(), nodes[j]->getId()});

                bool isSide = true;
                Vector3d* direction = nullptr;
                for (auto k = 0; k < nodes.size(); k++) {
                    if (k != i && k != j) {
                        Line* kLine = new Line({nodes[i]->getId(), nodes[k]->getId()});
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
                    if (isSideElementsRequired) {
                        Vector3d v1 = nodes[j]->getPosition() - nodes[i]->getPosition();
                        int p = 0;
                        for (auto k = 0; k < nodes.size(); k++) {
                            if (k != i && k != j) {
                                p = k;
                                break;
                            }
                        }
                        Vector3d v2 = nodes[p]->getPosition() - nodes[i]->getPosition();

                        _sideElements.emplace_back(new SideElement(line, -v1.vector(v2).vector(v1).normalize()));
                    }
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
