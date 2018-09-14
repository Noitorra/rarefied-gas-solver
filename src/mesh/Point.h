#ifndef RGS_POINT_H
#define RGS_POINT_H

#include "Element.h"

class Point : public Element {
    friend class boost::serialization::access;

public:
    Point() = default;

    Point(int id, int physicalEntityId, int geomUnitId, const std::vector<int>& partitions, const std::vector<int>& nodeIds)
    : Element(Type::POINT, id, physicalEntityId, geomUnitId, partitions, nodeIds) {}

    explicit Point(const std::vector<int>& nodeIds) : Point(0, -1, -1, {}, nodeIds) {}

private:
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & boost::serialization::base_object<Element>(*this);
    }

    void innerInit(const std::vector<Node*>& nodes, bool isSideElementsRequired) override {
        _volume = 1.0;
    }
};

#endif //RGS_POINT_H
