#ifndef RGS_ELEMENT_H
#define RGS_ELEMENT_H

#include "Node.h"
#include "SideElement.h"

#include <vector>
#include <memory>
#include <unordered_map>
#include <boost/serialization/shared_ptr.hpp>

class Element {
    friend class boost::serialization::access;

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
    int _physicalEntityId;
    int _geomUnitId;
    std::vector<int> _partitions;
    std::vector<int> _nodeIds;

    double _volume;
    std::vector<std::shared_ptr<SideElement>> _sideElements;

public:
    Element() = default;

    Element(Type type, int id, int physicalEntityId, int geomUnitId, std::vector<int> partitions, std::vector<int> nodeIds)
    : _type(type), _id(id), _physicalEntityId(physicalEntityId), _geomUnitId(geomUnitId), _partitions(std::move(partitions)), _nodeIds(std::move(nodeIds)) {}

    void init(const std::unordered_map<int, Node*>& allNodesMap) {
        std::vector<Node*> nodes;
        for (auto nodeId : _nodeIds) {
            nodes.push_back(allNodesMap.at(nodeId));
        }

        createSideElements(nodes);

        for (const auto& sideElement : _sideElements) {
            sideElement->getElement()->init(allNodesMap);
        }
    }

    bool containsSide(SideElement* otherSideElement) const {
        if (_sideElements.empty() == false) {
            std::vector<int> otherNodeIds = otherSideElement->getElement()->getNodeIds();
            std::sort(otherNodeIds.begin(), otherNodeIds.end());
            for (const auto& sideElement : _sideElements) {
                std::vector<int> nodeIds = sideElement->getElement()->getNodeIds();
                std::sort(nodeIds.begin(), nodeIds.end());
                if (nodeIds == otherNodeIds) {
                    return true;
                }
            }
        }
        return false;
    }

    Type getType() const {
        return _type;
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

    double getVolume() const {
        return _volume;
    }

    const std::vector<std::shared_ptr<SideElement>>& getSideElements() const {
        return _sideElements;
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

        ar & _volume;
        ar & _sideElements;
    }

    virtual void createSideElements(const std::vector<Node*>& nodes) = 0;

};


#endif //RGS_ELEMENT_H
