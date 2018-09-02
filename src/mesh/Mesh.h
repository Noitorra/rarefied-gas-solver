#include <utility>

//
// Created by Dmitry Sherbakov on 12.08.2018.
//

#ifndef TESTING_MESH_H
#define TESTING_MESH_H

#include <vector>
#include <map>
#include <memory>
#include "Node.h"
#include "Element.h"

class Mesh {
private:
    std::vector<std::shared_ptr<Node>> _nodes;
    std::map<int, Node*> _nodesMap;
    std::vector<std::shared_ptr<Element>> _elements;

public:
    Mesh() = default;

    std::size_t getNodesCapacity() const {
        return _nodes.capacity();
    }

    void setNodesCapacity(std::size_t capacity) {
        _nodes.reserve(capacity);
    }

    std::size_t getNodesCount() const {
        return _nodes.size();
    }

    void addNode(int id, Vector3d position) {
        Node* node = new Node(id, std::move(position));
        _nodes.emplace_back(node);
        _nodesMap.emplace(node->getId(), node);
    }

    Node* getNodeById(int id) const {
        return _nodesMap.at(id);
    }

    Node* getNode(int index) const {
        return _nodes[index].get();
    }

    std::size_t getElementsCapacity() const {
        return _elements.capacity();
    }

    void setElementsCapacity(std::size_t capacity) {
        _elements.reserve(capacity);
    }

    std::size_t getElementsCount() const {
        return _elements.size();
    }

    void addElement(int id, int type, const std::vector<int>& tags, const std::vector<int>& nodeIds);

    Element* getElement(int index) {
        return _elements[index].get();
    }

    const std::vector<std::shared_ptr<Element>>& getElements() const;

private:
    std::vector<Node*> getNodes(const std::vector<int>& nodeIds);

};


#endif //TESTING_MESH_H
