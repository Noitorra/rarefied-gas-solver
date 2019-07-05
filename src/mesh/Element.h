#ifndef RGS_ELEMENT_H
#define RGS_ELEMENT_H

#include "PhysicalEntity.h"
#include "Node.h"
#include "SideElement.h"

#include <vector>
#include <memory>
#include <map>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/string.hpp>
#include <iostream>

class Element {
    friend class boost::serialization::access;

public:
    enum class Type {
        POINT = 15,
        LINE = 1,
        TRIANGLE = 2,
        QUADRANGLE = 3,
        TETRAHEDRON = 4,
        HEXAHEDRON = 5,
        PRISM = 6
    };

protected:
    Type _type;
    int _id;
    int _physicalEntityId;
    int _geomUnitId;
    std::vector<int> _partitions;
    std::vector<int> _nodeIds;

    std::string _group;
    bool _isProcessing;
    bool _isMain;
    bool _isBorder;

    double _volume;
    std::vector<std::shared_ptr<SideElement>> _sideElements;

    Vector3d _center;

public:
    Element() = default;

    Element(Type type, int id, int physicalEntityId, int geomUnitId, std::vector<int> partitions, std::vector<int> nodeIds)
    : _type(type), _id(id), _physicalEntityId(physicalEntityId), _geomUnitId(geomUnitId), _partitions(std::move(partitions)),
    _nodeIds(std::move(nodeIds)), _group(""), _isProcessing(false), _isMain(false), _isBorder(false), _volume(0.0), _center(0, 0, 0) {}

    void init(const std::map<int, Node*>& allNodesMap, bool isOriginalElement) {
        std::vector<Node*> nodes;
        for (auto nodeId : _nodeIds) {
            nodes.push_back(allNodesMap.at(nodeId));
        }

        innerInit(nodes, isOriginalElement);

        if (isOriginalElement == true) {
            for (const auto& sideElement : _sideElements) {
                sideElement->getElement()->init(allNodesMap, false);
            }
        }

        _center.set(0, 0, 0);
        for (const auto& node : nodes) {
            _center += node->getPosition();
        }
        if (nodes.empty() == false) {
            _center /= nodes.size();
        }
    }

    bool isSideOrContainsSide(SideElement* otherSideElement) const {
        std::vector<int> otherNodeIds = otherSideElement->getElement()->getNodeIds();
        std::sort(otherNodeIds.begin(), otherNodeIds.end());

        // check if this element is exactly side
        std::vector<int> nodeIds = _nodeIds;
        std::sort(nodeIds.begin(), nodeIds.end());
        if (nodeIds == otherNodeIds) {
            return true;
        }

        // check if elements on same side of space
        Vector3d direction = _center - otherSideElement->getElement()->getCenter();
        if (direction.scalar(otherSideElement->getNormal()) < 0) {
            return false;
        }

        // check if this element has side with the same nodes
        if (_sideElements.empty() == false) {
            for (const auto& sideElement : _sideElements) {
                std::vector<int> sideNodeIds = sideElement->getElement()->getNodeIds();
                std::sort(sideNodeIds.begin(), sideNodeIds.end());
                if (sideNodeIds == otherNodeIds) {
                    return true;
                }
            }
        }

        return false;
    }

    Type getType() const {
        return _type;
    }

    bool is1D() {
        return _type == Type::LINE;
    }

    bool is2D() {
        return _type == Type::TRIANGLE || _type == Type::QUADRANGLE;
    }

    bool is3D() {
        return _type == Type::TETRAHEDRON || _type == Type::HEXAHEDRON || _type == Type::PRISM;
    }

    int getId() const {
        return _id;
    }

    int getPhysicalEntityId() const {
        return _physicalEntityId;
    }

    int getGeomUnitId() const {
        return _geomUnitId;
    }

    const std::vector<int>& getPartitions() const {
        return _partitions;
    }

    int getProcessId() const {
        return !_partitions.empty() ? (_partitions[0] - 1) : -1;
    }

    const std::vector<int>& getNodeIds() const {
        return _nodeIds;
    }

    const std::string& getGroup() const {
        return _group;
    }

    void setGroup(const std::string& group) {
        _group = group;
        _isMain = _group.find("Main") == 0;
        _isBorder = _group.find("Border") == 0;
        _isProcessing = _isMain || _isBorder;
    }

    bool isProcessing() const {
        return _isProcessing;
    }

    bool isMain() const {
        return _isMain;
    }

    bool isBorder() const {
        return _isBorder;
    }

    double getVolume() const {
        return _volume;
    }

    const std::vector<std::shared_ptr<SideElement>>& getSideElements() const {
        return _sideElements;
    }

    const Vector3d& getCenter() const {
        return _center;
    }

private:
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & _type;
        ar & _id;
        ar & _physicalEntityId;
        ar & _geomUnitId;
        ar & _partitions;
        ar & _nodeIds;

        ar & _group;
        ar & _isProcessing;
        ar & _isMain;
        ar & _isBorder;

        ar & _volume;
        ar & _sideElements;

        ar & _center;
    }

    virtual void innerInit(const std::vector<Node*>& nodes, bool isSideElementsRequired) = 0;

};


#endif //RGS_ELEMENT_H
