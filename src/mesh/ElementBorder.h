#ifndef RGS_ELEMENTBORDER_H
#define RGS_ELEMENTBORDER_H

#include "utilities/Types.h"

#include <memory>

class Element;

class ElementBorder {
    friend class boost::serialization::access;

private:
    std::shared_ptr<Element> _element;
    Vector3d _normal;
    int _neighborId;

public:
    ElementBorder() = default;

    ElementBorder(Element* element, Vector3d normal) : _element(element), _normal(std::move(normal)), _neighborId(0) {}

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

private:
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & _element;
        ar & _normal;
        ar & _neighborId;
    }

};

#endif //RGS_ELEMENTBORDER_H
