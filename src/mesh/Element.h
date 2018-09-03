#ifndef RGS_ELEMENT_H
#define RGS_ELEMENT_H

#include <vector>
#include <memory>
#include "Node.h"

class Element {
public:
    enum class Type {
        POINT = 15,
        LINE = 1,
        TRIANGLE = 2,
        QUADRANGLE = 3
    };

protected:
    Type _type;
    int _id;
    std::vector<Node*> _nodes;
    std::vector<std::shared_ptr<Element>> _sideElements;
    double _volume;
    Vector3d _normal;

public:
    Element(Type type, int id, const std::vector<Node*>& nodes) : _type(type), _id(id), _nodes(nodes), _volume(1.0) {}

    Type getType() const {
        return _type;
    }

    int getId() const {
        return _id;
    }

    const std::vector<Node*>& getNodes() const {
        return _nodes;
    }

    const std::vector<std::shared_ptr<Element>>& getSideElements() const {
        return _sideElements;
    }

    double getVolume() const {
        return _volume;
    }

    const Vector3d& getNormal() const {
        return _normal;
    }

    void setNormal(const Vector3d& normal) {
        _normal = normal;
    }

    bool contains(const std::vector<Node*>& nodes) const {
        for (const auto& node : nodes) {
            auto result = std::find_if(_nodes.begin(), _nodes.end(), [&node](Node* value) {
                return value->getId() == node->getId();
            });
            if (result == _nodes.end()) {
                return false;
            }
        }
        return true;
    }
};


#endif //RGS_ELEMENT_H
