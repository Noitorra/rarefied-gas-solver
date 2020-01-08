#ifndef RGS_LINE_H
#define RGS_LINE_H

#include "Element.h"
#include "Point.h"

#include <cmath>

class Line : public Element {
    friend class boost::serialization::access;

public:
    Line() = default;

    Line(int id, int physicalEntityId, int geomUnitId, const std::vector<int>& partitions, const std::vector<int>& nodeIds)
    : Element(Type::LINE, id, physicalEntityId, geomUnitId, partitions, nodeIds) {}

    explicit Line(const std::vector<int>& nodeIds) : Line(0, -1, -1, {}, nodeIds) {}

private:
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & boost::serialization::base_object<Element>(*this);
    }

    void innerInit(const std::vector<Node*>& nodes, bool isSideElementsRequired) override {
        Vector3d a = nodes[1]->getPosition() - nodes[0]->getPosition();

        if (isSideElementsRequired) {
            _borderElements.clear();
            _borderElements.emplace_back(new ElementBorder(new Point({nodes[0]->getId()}), -Vector3d(a).normalize()));
            _borderElements.emplace_back(new ElementBorder(new Point({nodes[1]->getId()}), Vector3d(a).normalize()));
        }

        _volume = a.module();
    }
};

#endif //RGS_LINE_H
