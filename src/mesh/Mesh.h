#ifndef RGS_MESH_H
#define RGS_MESH_H

#include "PhysicalEntity.h"
#include "Node.h"
#include "Element.h"

#include <vector>
#include <map>
#include <memory>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/map.hpp>

class Mesh {
    friend class boost::serialization::access;

private:
    std::vector<std::shared_ptr<PhysicalEntity>> _physicalEntities;
    std::map<int, PhysicalEntity*> _physicalEntitiesMap;
    std::vector<std::shared_ptr<Node>> _nodes;
    std::map<int, Node*> _nodesMap;
    std::vector<std::shared_ptr<Element>> _elements;
    std::map<int, Element*> _elementsMap;

public:
    Mesh() = default;

    void init();

    void resetMaps();

    void reservePhysicalEntities(std::size_t capacity);

    void addPhysicalEntity(int dimension, int id, std::string name);

    void addPhysicalEntity(PhysicalEntity* physicalEntity);

    PhysicalEntity* getPhysicalEntity(int id) const;

    const std::vector<std::shared_ptr<PhysicalEntity>>& getPhysicalEntities() const;

    void reserveNodes(std::size_t capacity);

    void addNode(int id, Vector3d position);

    void addNode(Node* node);

    Node* getNode(int id) const;

    const std::vector<std::shared_ptr<Node>>& getNodes() const;

    void reserveElements(std::size_t capacity);

    void addElement(int id, int type, int physicalEntityId, int geomUnitId, const std::vector<int>& partitions, const std::vector<int>& nodeIds);

    void addElement(Element* element);

    Element* getElement(int id) const;

    const std::vector<std::shared_ptr<Element>>& getElements() const;

private:
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & _physicalEntities;
        ar & _nodes;
        ar & _elements;
    }

};


#endif //RGS_MESH_H
