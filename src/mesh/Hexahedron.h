#ifndef RGS_HEXAHEDRON_H
#define RGS_HEXAHEDRON_H

#include "Element.h"
#include "Quadrangle.h"

class Hexahedron : public Element {
    friend class boost::serialization::access;

public:
    Hexahedron() = default;

    Hexahedron(int id, int physicalEntityId, int geomUnitId, const std::vector<int>& partitions, const std::vector<int>& nodeIds)
    : Element(Type::HEXAHEDRON, id, physicalEntityId, geomUnitId, partitions, nodeIds) {}

    explicit Hexahedron(const std::vector<int>& nodeIds) : Hexahedron(0, -1, -1, {}, nodeIds) {}

private:
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & boost::serialization::base_object<Element>(*this);
    }

    void innerInit(const std::vector<Node*>& nodes, bool isSideElementsRequired) override {
        if (isSideElementsRequired) {
            Vector3d a = nodes[1]->getPosition() - nodes[0]->getPosition();
            Vector3d b = nodes[3]->getPosition() - nodes[0]->getPosition();
            Vector3d c = nodes[4]->getPosition() - nodes[0]->getPosition();

            Vector3d d = nodes[2]->getPosition() - nodes[6]->getPosition();
            Vector3d e = nodes[7]->getPosition() - nodes[6]->getPosition();
            Vector3d f = nodes[5]->getPosition() - nodes[6]->getPosition();

            _sideElements.clear();

            _sideElements.emplace_back(new SideElement(
                    new Quadrangle({nodes[0]->getId(), nodes[1]->getId(), nodes[2]->getId(), nodes[3]->getId()}),
                    b.vector(a).normalize()
            ));
            _sideElements.emplace_back(new SideElement(
                    new Quadrangle({nodes[0]->getId(), nodes[4]->getId(), nodes[5]->getId(), nodes[1]->getId()}),
                    a.vector(c).normalize()
            ));
            _sideElements.emplace_back(new SideElement(
                    new Quadrangle({nodes[0]->getId(), nodes[4]->getId(), nodes[7]->getId(), nodes[3]->getId()}),
                    c.vector(b).normalize()
            ));

            _sideElements.emplace_back(new SideElement(
                    new Quadrangle({nodes[6]->getId(), nodes[2]->getId(), nodes[1]->getId(), nodes[5]->getId()}),
                    f.vector(d).normalize()
            ));
            _sideElements.emplace_back(new SideElement(
                    new Quadrangle({nodes[6]->getId(), nodes[2]->getId(), nodes[3]->getId(), nodes[7]->getId()}),
                    d.vector(e).normalize()
            ));
            _sideElements.emplace_back(new SideElement(
                    new Quadrangle({nodes[6]->getId(), nodes[7]->getId(), nodes[4]->getId(), nodes[5]->getId()}),
                    e.vector(f).normalize()
            ));
        }

        // volume is summary of 3 pyramids volume (1/3 * base x height)
        double volume = 0.0;

        // first pyramid
        Vector3d v40 = nodes[4]->getPosition() - nodes[0]->getPosition();
        Vector3d v30 = nodes[3]->getPosition() - nodes[0]->getPosition();
        Vector3d v47 = nodes[4]->getPosition() - nodes[7]->getPosition();
        Vector3d v37 = nodes[3]->getPosition() - nodes[7]->getPosition();
        Vector3d v10 = nodes[1]->getPosition() - nodes[0]->getPosition();
        volume += (v40.vector(v30).module() + v47.vector(v37).module()) * v10.scalar(v30.vector(v40).normalize()) / 6;

        // second pyramid
        Vector3d v32 = nodes[3]->getPosition() - nodes[2]->getPosition();
        Vector3d v62 = nodes[6]->getPosition() - nodes[2]->getPosition();
        Vector3d v67 = nodes[6]->getPosition() - nodes[7]->getPosition();
        Vector3d v12 = nodes[1]->getPosition() - nodes[2]->getPosition();
        volume += (v32.vector(v62).module() + v37.vector(v67).module()) * v12.scalar(v32.vector(v62).normalize()) / 6;

        // third pyramid
        Vector3d v65 = nodes[6]->getPosition() - nodes[5]->getPosition();
        Vector3d v45 = nodes[4]->getPosition() - nodes[5]->getPosition();
        Vector3d v15 = nodes[1]->getPosition() - nodes[5]->getPosition();
        volume += (v65.vector(v45).module() + v67.vector(v47).module()) * v15.scalar(v65.vector(v45).normalize()) / 6;

        _volume = volume;
    }

};

#endif //RGS_HEXAHEDRON_H
