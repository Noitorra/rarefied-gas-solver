#ifndef RGS_LINE_H
#define RGS_LINE_H

#include "Element.h"
#include "Point.h"

#include <cmath>

class Line : public Element {
    friend class boost::serialization::access;

private:
    Vector3d _vector;

public:
    Line() = default;

    Line(int id, int physicalEntityId, int geomUnitId, const std::vector<int>& partitions, const std::vector<int>& nodeIds)
    : Element(Type::LINE, id, physicalEntityId, geomUnitId, partitions, nodeIds) {}

    explicit Line(const std::vector<int>& nodeIds) : Line(0, -1, -1, {}, nodeIds) {}

    const Vector3d& getVector() const {
        return _vector;
    }

private:
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & boost::serialization::base_object<Element>(*this);
        ar & _vector;
    }

    void innerInit(const std::vector<Node*>& nodes, bool isSideElementsRequired) override {
        _vector = nodes[1]->getPosition() - nodes[0]->getPosition();

        if (isSideElementsRequired) {
            _sideElements.clear();
            _sideElements.emplace_back(new SideElement(new Point({nodes[0]->getId()}), -Vector3d(_vector).normalize()));
            _sideElements.emplace_back(new SideElement(new Point({nodes[1]->getId()}), Vector3d(_vector).normalize()));
        }

        _volume = _vector.module();
    }
};

#endif //RGS_LINE_H
