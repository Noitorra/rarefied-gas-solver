#include "Mesh.h"
#include "Point.h"
#include "Line.h"
#include "Triangle.h"
#include "Quadrangle.h"

void Mesh::addElement(int id, int type, const std::vector<int>& tags, const std::vector<int>& nodeIds) {
    Element* element = nullptr;
    switch (static_cast<Element::Type>(type)) {
        case Element::Type::POINT:
            element = new Point(id, getNodes(nodeIds));
            break;
        case Element::Type::LINE:
            element = new Line(id, getNodes(nodeIds));
            break;
        case Element::Type::TRIANGLE:
            element = new Triangle(id, getNodes(nodeIds));
            break;
        case Element::Type::QUADRANGLE:
            element = new Quadrangle(id, getNodes(nodeIds));
            break;
    }
    _elements.emplace_back(element);
}

std::vector<Node*> Mesh::getNodes(const std::vector<int>& nodeIds) {
    std::vector<Node*> nodes;
    for (const auto& nodeId : nodeIds) {
        nodes.push_back(getNodeById(nodeId));
    }
    return nodes;
}

const std::vector<std::shared_ptr<Element>>& Mesh::getElements() const {
    return _elements;
}
