#ifndef RGS_MESH_H
#define RGS_MESH_H

#include "Node.h"
#include "Element.h"

#include <vector>
#include <unordered_map>
#include <memory>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <bits/unordered_map.h>

class Mesh {
    friend class boost::serialization::access;

private:
    std::vector<std::shared_ptr<Node>> _nodes;
    std::unordered_map<int, Node*> _nodesMap;
    std::vector<std::shared_ptr<Element>> _elements;

public:
    Mesh() = default;

    void init();

    std::unordered_map<int, Mesh*> split();

    void reserveNodes(std::size_t capacity);

    void addNode(int id, Vector3d position);

    void addNode(Node* node);

    Node* getNode(int id) const;

    const std::vector<std::shared_ptr<Node>>& getNodes() const;

    void reserveElements(std::size_t capacity);

    std::size_t getElementsCount() const;

    void addElement(int id, int type, int physicalEntityId, int geomUnitId, const std::vector<int>& partitions, const std::vector<int>& nodeIds);

    void addElement(Element* element);

    Element* getElement(int index) const;

    const std::vector<std::shared_ptr<Element>>& getElements() const;

private:
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & _nodesMap;
        ar & _elements;
    }

};


#endif //RGS_MESH_H
