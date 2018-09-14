#include "Mesh.h"
#include "Point.h"
#include "Line.h"
#include "Triangle.h"
#include "Quadrangle.h"
#include "Tetrahedron.h"
#include "utilities/Utils.h"
#include "utilities/Config.h"
#include "utilities/Normalizer.h"

#include <iostream>

void Mesh::init() {
    for (const auto& element : _elements) {
        element->init(_nodesMap, true);
    }

    // preprocess mesh (find all neighbors)
    for (const auto& element : _elements) {
        if (element->getType() != Element::Type::LINE) {
            continue;
        }

        // find all neighbors (common nodes)
        for (const auto& sideElement : element->getSideElements()) {

            // each side element can have neighbor or don't have one
            auto predicate = [&element, &sideElement](const std::shared_ptr<Element>& otherElement) {
                return otherElement->getId() != element->getId() && otherElement->containsSide(sideElement.get());
            };
            auto result = std::find_if(std::begin(_elements), std::end(_elements), predicate);
            if (result != std::end(_elements)) {
                const auto& otherElement = *result;
                if (element->getProcessId() == otherElement->getProcessId()) {
                    sideElement->setNeighborId(otherElement->getId());
                } else {
                    sideElement->setNeighborId(-otherElement->getId());
                }
                sideElement->setNeighborProcessId(otherElement->getProcessId());
            } else {
                sideElement->setNeighborId(0);
                sideElement->setNeighborProcessId(-1);
            }
        }
    }

    std::vector<std::pair<int, int>> ids, neighborIds;
    for (const auto& element : _elements) {
        for (const auto& sideElement : element->getSideElements()) {
            if (sideElement->getNeighborId() < 0) {
                ids.emplace_back(element->getProcessId(), element->getId());
                neighborIds.emplace_back(sideElement->getNeighborProcessId(), -sideElement->getNeighborId());
            }
        }
    }
    std::sort(ids.begin(), ids.end(), [](const std::pair<int, int>& left, const std::pair<int, int>& right) -> bool {
        return left.first < right.first || (left.first == right.first && left.second < right.second);
    });
    std::sort(neighborIds.begin(), neighborIds.end(), [](const std::pair<int, int>& left, const std::pair<int, int>& right) -> bool {
        return left.first < right.first || (left.first == right.first && left.second < right.second);
    });

    for (auto i = 0; i < ids.size() || i < neighborIds.size(); i++) {
        int process = -1, id = 0;
        if (i < ids.size()) {
            process = ids[i].first;
            id = ids[i].second;
            std::cout << process << " : " << id;
        }
        std::cout << " <=> ";
        int neighborProcess = -1, neighborId = 0;
        if (i < neighborIds.size()) {
            neighborProcess = neighborIds[i].first;
            neighborId = neighborIds[i].second;
            std::cout << neighborIds[i].first << " : " << neighborIds[i].second;
        }
        std::cout << std::endl;

        if (process != neighborProcess || id != neighborId) {
            throw std::runtime_error("Wrong partitioning!!!");
        }
    }
    std::cout << std::endl;
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
        _nodesMap[_nodes.back()->getId()] = _nodes.back().get();
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
        case Element::Type::TETRAHEDRON:
            element = new Tetrahedron(id, physicalEntityId, geomUnitId, partitions, nodeIds);
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
