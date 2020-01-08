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
        Vector3d a = nodes[1]->getPosition() - nodes[0]->getPosition();
        Vector3d b = nodes[2]->getPosition() - nodes[1]->getPosition();
        Vector3d c = nodes[0]->getPosition() - nodes[2]->getPosition();

        if (isSideElementsRequired) {
            _borderElements.clear();
            _borderElements.emplace_back(new ElementBorder(new Line({nodes[0]->getId(), nodes[1]->getId()}), -a.vector(b).vector(a).normalize()));
            _borderElements.emplace_back(new ElementBorder(new Line({nodes[1]->getId(), nodes[2]->getId()}), -b.vector(c).vector(b).normalize()));
            _borderElements.emplace_back(new ElementBorder(new Line({nodes[2]->getId(), nodes[0]->getId()}), -c.vector(a).vector(c).normalize()));
        }

        _volume = a.vector(b).module() / 2;
    }
};

#endif //RGS_TRIANGLE_H
