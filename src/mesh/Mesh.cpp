#include "Mesh.h"
#include "Point.h"
#include "Line.h"
#include "Triangle.h"
#include "Quadrangle.h"
#include "Tetrahedron.h"
#include "Hexahedron.h"
#include "Prism.h"
#include "utilities/Utils.h"
#include "utilities/Normalizer.h"
#include "core/Config.h"

#include <iostream>
#include <stdexcept>

void Mesh::init() {

    // pre-process elements (find physical entities)
    for (const auto& element : _elements) {
        auto entityId = element->getPhysicalEntityId();
        auto entity = _physicalEntitiesMap.at(entityId);

        // element without entity (cannot setup initial or border params, junk element)
        if (entity == nullptr) {
            continue;
        }

        element->setGroup(entity->getName());
    }

    // create side elements and calculate volume
    for (const auto& element : _elements) {
        if (element->isMain() == true) {
            element->init(_nodesMap, true);
        }
    }

    // pre-process mesh (find all neighbors) (very slow)
    for (const auto& element : _elements) {
        if (element->isMain() == false) {
            continue;
        }

        // find all neighbors (common nodes)
        for (const auto& sideElement : element->getSideElements()) {

            // each side element can have neighbor or don't have one
            auto predicate = [&element, &sideElement](const std::shared_ptr<Element>& otherElement) {
                return otherElement->getId() != element->getId() && otherElement->isSideOrContainsSide(sideElement.get());
            };
            auto result = std::find_if(_elements.begin(), _elements.end(), predicate);
            if (result != _elements.end()) {
                const auto& otherElement = *result;
                sideElement->setNeighborId(otherElement->getId());
            } else {
                std::ostringstream os;
                os << "main element doesn't have any neighbors" << ", id = " << element->getId();
                std::string message = os.str();
                throw std::runtime_error(message);
            }
        }
    }
}

void Mesh::resetMaps() {
    _physicalEntitiesMap.clear();
    for (const auto& entity : _physicalEntities) {
        _physicalEntitiesMap[entity->getId()] = entity.get();
    }

    _nodesMap.clear();
    for (const auto& node : _nodes) {
        _nodesMap[node->getId()] = node.get();
    }

    _elementsMap.clear();
    for (const auto& element : _elements) {
        _elementsMap[element->getId()] = element.get();
    }
}

void Mesh::reservePhysicalEntities(std::size_t capacity) {
    _physicalEntities.reserve(capacity);
}

void Mesh::addPhysicalEntity(int dimension, int id, std::string name) {
   addPhysicalEntity(new PhysicalEntity(dimension, id, std::move(name)));
}

void Mesh::addPhysicalEntity(PhysicalEntity* physicalEntity) {
    if (_physicalEntitiesMap[physicalEntity->getId()] == nullptr) {
        _physicalEntitiesMap[physicalEntity->getId()] = physicalEntity;
        _physicalEntities.emplace_back(physicalEntity);
    }
}

PhysicalEntity* Mesh::getPhysicalEntity(int id) const {
    return _physicalEntitiesMap.at(id);
}

const std::vector<std::shared_ptr<PhysicalEntity>>& Mesh::getPhysicalEntities() const {
    return _physicalEntities;
}

void Mesh::reserveNodes(std::size_t capacity) {
    _nodes.reserve(capacity);
}

void Mesh::addNode(int id, Vector3d position) {
    addNode(new Node(id, std::move(position)));
}

void Mesh::addNode(Node* node) {
    if (_nodesMap[node->getId()] == nullptr) {
        _nodesMap[node->getId()] = node;
        _nodes.emplace_back(node);
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
        case Element::Type::HEXAHEDRON:
            element = new Hexahedron(id, physicalEntityId, geomUnitId, partitions, nodeIds);
            break;
        case Element::Type::PRISM:
            element = new Prism(id, physicalEntityId, geomUnitId, partitions, nodeIds);
            break;
    }
    addElement(element);
}

void Mesh::addElement(Element* element) {
    if (_elementsMap[element->getId()] == nullptr) {
        _elementsMap[element->getId()] = element;
        _elements.emplace_back(element);
    }
}

Element* Mesh::getElement(int id) const {
    return _elementsMap.at(id);
}

const std::vector<std::shared_ptr<Element>>& Mesh::getElements() const {
    return _elements;
}
