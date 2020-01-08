#ifndef RGS_PRISM_H
#define RGS_PRISM_H

#include "Element.h"
#include "Triangle.h"
#include "Quadrangle.h"

class Prism : public Element {
    friend class boost::serialization::access;

public:
    Prism() = default;

    Prism(int id, int physicalEntityId, int geomUnitId, const std::vector<int>& partitions, const std::vector<int>& nodeIds)
    : Element(Type::PRISM, id, physicalEntityId, geomUnitId, partitions, nodeIds) {}

    explicit Prism(const std::vector<int>& nodeIds) : Prism(0, -1, -1, {}, nodeIds) {}

private:
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & boost::serialization::base_object<Element>(*this);
    }

    void innerInit(const std::vector<Node*>& nodes, bool isSideElementsRequired) override {
        Vector3d v10 = nodes[1]->getPosition() - nodes[0]->getPosition();
        Vector3d v20 = nodes[2]->getPosition() - nodes[0]->getPosition();
        Vector3d v21 = nodes[2]->getPosition() - nodes[1]->getPosition();
        Vector3d v30 = nodes[3]->getPosition() - nodes[0]->getPosition();
        Vector3d v43 = nodes[4]->getPosition() - nodes[3]->getPosition();
        Vector3d v53 = nodes[5]->getPosition() - nodes[3]->getPosition();

        if (isSideElementsRequired) {
            _borderElements.clear();

            // up and down sides
            _borderElements.emplace_back(new ElementBorder(
                    new Triangle({nodes[0]->getId(), nodes[1]->getId(), nodes[2]->getId()}),
                    v20.vector(v10).normalize()
            ));
            _borderElements.emplace_back(new ElementBorder(
                    new Triangle({nodes[3]->getId(), nodes[4]->getId(), nodes[5]->getId()}),
                    v43.vector(v53).normalize()
            ));

            // other sides
            _borderElements.emplace_back(new ElementBorder(
                    new Quadrangle({nodes[0]->getId(), nodes[1]->getId(), nodes[4]->getId(), nodes[3]->getId()}),
                    v10.vector(v30).normalize()
            ));
            _borderElements.emplace_back(new ElementBorder(
                    new Quadrangle({nodes[0]->getId(), nodes[2]->getId(), nodes[5]->getId(), nodes[3]->getId()}),
                    v30.vector(v20).normalize()
            ));
            _borderElements.emplace_back(new ElementBorder(
                    new Quadrangle({nodes[1]->getId(), nodes[2]->getId(), nodes[5]->getId(), nodes[4]->getId()}),
                    v21.vector(v30).normalize()
            ));
        }

        _volume = v10.vector(v20).module() * v30.module() / 2;
    }

};

#endif //RGS_PRISM_H
