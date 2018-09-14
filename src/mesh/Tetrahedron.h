#ifndef RGS_TETRAHEDRON_H
#define RGS_TETRAHEDRON_H

#include "Element.h"
#include "Triangle.h"

class Tetrahedron : public Element {
    friend class boost::serialization::access;

public:
    Tetrahedron() = default;

    Tetrahedron(int id, int physicalEntityId, int geomUnitId, const std::vector<int>& partitions, const std::vector<int>& nodeIds)
    : Element(Type::TETRAHEDRON, id, physicalEntityId, geomUnitId, partitions, nodeIds) {}

    explicit Tetrahedron(const std::vector<int>& nodeIds) : Tetrahedron(0, -1, -1, {}, nodeIds) {}

private:
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & boost::serialization::base_object<Element>(*this);
    }

    void innerInit(const std::vector<Node*>& nodes, bool isSideElementsRequired) override {
        if (isSideElementsRequired) {
            _sideElements.clear();
            for (auto i = 0; i < nodes.size(); i++) {
                for (auto j = i + 1; j < nodes.size(); j++) {
                    for (auto k = j + 1; k < nodes.size(); k++) {
                        Element* triangle = new Triangle({nodes[i]->getId(), nodes[j]->getId(), nodes[k]->getId()});
                        Vector3d v1 = nodes[j]->getPosition() - nodes[i]->getPosition();
                        Vector3d v2 = nodes[k]->getPosition() - nodes[i]->getPosition();
                        Vector3d v3;
                        for (auto l = 0; l < nodes.size(); l++) {
                            if (l != i && l != j && l != k) {
                                v3 = nodes[l]->getPosition() - nodes[i]->getPosition();
                                break;
                            }
                        }
                        Vector3d normal = v1.vector(v2).normalize();
                        if (normal.scalar(v3) > 0) {
                            normal = -normal;
                        }
                        _sideElements.emplace_back(new SideElement(triangle, normal));
                    }
                }
            }
        }

        Vector3d a = nodes[0]->getPosition();
        Vector3d b = nodes[1]->getPosition();
        Vector3d c = nodes[2]->getPosition();
        Vector3d d = nodes[3]->getPosition();
        _volume = std::abs((a - d).scalar((b - d).vector(c - d))) / 6;
    }

};

#endif //RGS_TETRAHEDRON_H
