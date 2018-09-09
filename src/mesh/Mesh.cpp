#include "Mesh.h"
#include "Point.h"
#include "Line.h"
#include "Triangle.h"
#include "Quadrangle.h"

#include <iostream>
#include <utilities/Utils.h>
#include <bits/unordered_map.h>

void Mesh::init() {
    for (const auto& element : _elements) {
        element->init(_nodesMap);
    }

    // preprocess mesh (find all neighbors)
    for (const auto& element : _elements) {

        // find all neighbors (common nodes)
        for (const auto& sideElement : element->getSideElements()) {

            // each side element can have neighbor or don't have one
            auto result = std::find_if(std::begin(_elements), std::end(_elements), [&element, &sideElement](const std::shared_ptr<Element>& otherElement) {
                return otherElement->getId() != element->getId() && otherElement->containsSide(sideElement.get());
            });
            if (result != std::end(_elements)) {
                const auto& otherElement = *result;
                if (otherElement->getProcessId() >= 0) {
                    if (element->getProcessId() == otherElement->getProcessId()) {
                        sideElement->setNeighborId(otherElement->getId());
                    } else {
                        sideElement->setNeighborId(-otherElement->getId());
                    }
                    sideElement->setNeighborProcessId(otherElement->getProcessId());
                } else {
                    sideElement->setNeighborId(otherElement->getId());
                    sideElement->setNeighborProcessId(-1);
                }
            } else {
                sideElement->setNeighborId(0);
                sideElement->setNeighborProcessId(-1);
            }
        }
    }

//    for (const auto& element : _elements) {
//        if (element->getType() == Element::Type::TRIANGLE) {
//            for (const auto& sideElement : element->getSideElements()) {
//                if (sideElement->getNeighborId() < 0) {
//                    std::cout << sideElement->getNeighborProcessId() << " : " << sideElement->getNeighborId() << " : " << element->getId() << std::endl;
//                }
//            }
//        }
//    }
}

std::unordered_map<int, Mesh*> Mesh::split() {
    std::unordered_map<int, Mesh*> meshes;

    for (const auto& element : _elements) {
        int processId = element->getProcessId();
        if (processId < 0) {
            continue;
        }

        if (meshes[processId] == nullptr) {
            meshes[processId] = new Mesh();
        }
        Mesh* mesh = meshes[processId];

        mesh->addElement(element.get());
        for (auto nodeId : element->getNodeIds()) {
            mesh->addNode(_nodesMap[nodeId]);
        }
    }

    return meshes;
}

void Mesh::reserveNodes(std::size_t capacity) {
    _nodes.reserve(capacity);
    _nodesMap.reserve(capacity);
}

void Mesh::addNode(int id, Vector3d position) {
    addNode(new Node(id, std::move(position)));
}

void Mesh::addNode(Node* node) {
    if (_nodesMap[node->getId()] == nullptr) {
        _nodes.emplace_back(node);
        _nodesMap[node->getId()] = _nodes.back().get();
    }
}

Node* Mesh::getNode(int id) const {
    return _nodesMap.at(id);
}

const std::vector<std::shared_ptr<Node>>& Mesh::getNodes() const {
    return _nodes;
}

void Mesh::reserveElements(std::size_t capacity) {
    _elements.reserve(capacity);
}

std::size_t Mesh::getElementsCount() const {
    return _elements.size();
}

void Mesh::addElement(int id, int type, int physicalEntityId, int geomUnitId, const std::vector<int>& partitions, const std::vector<int>& nodeIds) {
    Element* element = nullptr;
    switch (static_cast<Element::Type>(type)) {
        case Element::Type::POINT:
            element = new Point(id, physicalEntityId, geomUnitId, partitions, nodeIds);
            break;
        case Element::Type::LINE:
            element = new Line(id, physicalEntityId, geomUnitId, partitions, nodeIds);
            break;
        case Element::Type::TRIANGLE:
            element = new Triangle(id, physicalEntityId, geomUnitId, partitions, nodeIds);
            break;
        case Element::Type::QUADRANGLE:
            element = new Quadrangle(id, physicalEntityId, geomUnitId, partitions, nodeIds);
            break;
    }
    _elements.emplace_back(element);
}

void Mesh::addElement(Element* element) {
    _elements.emplace_back(element);
}

Element* Mesh::getElement(int index) const {
    return _elements[index].get();
}

const std::vector<std::shared_ptr<Element>>& Mesh::getElements() const {
    return _elements;
}
