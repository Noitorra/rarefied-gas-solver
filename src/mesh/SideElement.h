#ifndef RGS_SIDEELEMENT_H
#define RGS_SIDEELEMENT_H

#include "utilities/Types.h"

#include <memory>

class Element;

class SideElement {
    friend class boost::serialization::access;

private:
    std::shared_ptr<Element> _element;
    Vector3d _normal;
    int _neighborId;
    int _neighborProcessId;

public:
    SideElement() = default;

    SideElement(Element* element, Vector3d normal) : _element(element), _normal(std::move(normal)), _neighborId(0), _neighborProcessId(-1) {}

    const std::shared_ptr<Element>& getElement() const {
        return _element;
    }

    const Vector3d& getNormal() const {
        return _normal;
    }

    int getNeighborId() const {
        return _neighborId;
    }

    void setNeighborId(int neighborId) {
        _neighborId = neighborId;
    }

    int getNeighborProcessId() const {
        return _neighborProcessId;
    }

    void setNeighborProcessId(int neighborProcessId) {
        _neighborProcessId = neighborProcessId;
    }

private:
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & _element;
        ar & _normal;
        ar & _neighborId;
        ar & _neighborProcessId;
    }

};

#endif //RGS_SIDEELEMENT_H
