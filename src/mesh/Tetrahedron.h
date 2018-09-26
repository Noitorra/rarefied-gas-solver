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
        Vector3d a = nodes[1]->getPosition() - nodes[0]->getPosition();
        Vector3d b = nodes[2]->getPosition() - nodes[1]->getPosition();
        Vector3d c = nodes[0]->getPosition() - nodes[2]->getPosition();
        Vector3d d = nodes[3]->getPosition() - nodes[1]->getPosition();
        Vector3d e = nodes[0]->getPosition() - nodes[3]->getPosition();

        if (isSideElementsRequired) {
            _sideElements.clear();
            _sideElements.emplace_back(new SideElement(new Triangle({nodes[0]->getId(), nodes[1]->getId(), nodes[2]->getId()}), -a.vector(b).normalize()));
            _sideElements.emplace_back(new SideElement(new Triangle({nodes[0]->getId(), nodes[1]->getId(), nodes[3]->getId()}), a.vector(d).normalize()));
            _sideElements.emplace_back(new SideElement(new Triangle({nodes[0]->getId(), nodes[2]->getId(), nodes[3]->getId()}), -c.vector(e).normalize()));
            _sideElements.emplace_back(new SideElement(new Triangle({nodes[1]->getId(), nodes[2]->getId(), nodes[3]->getId()}), b.vector(d).normalize()));
        }

        _volume = std::abs((a).scalar(c.vector(e))) / 6;
    }

};

#endif //RGS_TETRAHEDRON_H
