#ifndef RGS_TRIANGLE_H
#define RGS_TRIANGLE_H

#include "Element.h"
#include "Line.h"

#include <iostream>

class Triangle : public Element {
    friend class boost::serialization::access;

public:
    Triangle() = default;

    Triangle(int id, int physicalEntityId, int geomUnitId, const std::vector<int>& partitions, const std::vector<int>& nodeIds)
    : Element(Type::TRIANGLE, id, physicalEntityId, geomUnitId, partitions, nodeIds) {}

    explicit Triangle(const std::vector<int>& nodeIds) : Triangle(0, -1, -1, {}, nodeIds) {}

private:
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & boost::serialization::base_object<Element>(*this);
    }

    void innerInit(const std::vector<Node*>& nodes, bool isSideElementsRequired) override {
        Vector3d v1 = nodes[1]->getPosition() - nodes[0]->getPosition();
        Vector3d v2 = nodes[2]->getPosition() - nodes[0]->getPosition();
        Vector3d v3 = nodes[2]->getPosition() - nodes[1]->getPosition();

        if (isSideElementsRequired) {
            _sideElements.clear();
            _sideElements.emplace_back(new SideElement(new Line({nodes[0]->getId(), nodes[1]->getId()}), -v1.vector(v2).vector(v1).normalize()));
            _sideElements.emplace_back(new SideElement(new Line({nodes[1]->getId(), nodes[2]->getId()}), v3.vector(v1).vector(v3).normalize()));
            _sideElements.emplace_back(new SideElement(new Line({nodes[2]->getId(), nodes[0]->getId()}), v2.vector(v3).vector(v2).normalize()));
        }

        _volume = v1.vector(v2).module() / 2;
    }
};

#endif //RGS_TRIANGLE_H
