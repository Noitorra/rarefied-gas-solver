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
        if (isSideElementsRequired) {
            _sideElements.clear();

            Vector3d a = nodes[1]->getPosition() - nodes[0]->getPosition();
            Vector3d b = nodes[2]->getPosition() - nodes[1]->getPosition();
            Vector3d c = nodes[3]->getPosition() - nodes[2]->getPosition();
            Vector3d d = nodes[0]->getPosition() - nodes[3]->getPosition();

            _sideElements.emplace_back(new SideElement(new Line({nodes[0]->getId(), nodes[1]->getId()}), -a.vector(b).vector(a)));
            _sideElements.emplace_back(new SideElement(new Line({nodes[1]->getId(), nodes[2]->getId()}), -b.vector(c).vector(b)));
            _sideElements.emplace_back(new SideElement(new Line({nodes[2]->getId(), nodes[3]->getId()}), -c.vector(d).vector(c)));
            _sideElements.emplace_back(new SideElement(new Line({nodes[3]->getId(), nodes[0]->getId()}), -d.vector(a).vector(d)));
        }

        // volume is easy: d1d2sin(1,2) / 2
        Vector3d diag1 = nodes[2]->getPosition() - nodes[0]->getPosition();
        Vector3d diag2 = nodes[3]->getPosition() - nodes[1]->getPosition();
        _volume = diag1.vector(diag2).module() / 2;
    }
};

#endif //RGS_QUADRANGLE_H
