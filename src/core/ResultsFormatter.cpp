#include "ResultsFormatter.h"
#include "utilities/Normalizer.h"
#include "utilities/Utils.h"
#include "grid/Grid.h"
#include "grid/NormalCell.h"
#include "mesh/Mesh.h"

#include <boost/filesystem.hpp>
#include <iostream>

using namespace boost::filesystem;

ResultsFormatter::ResultsFormatter() {
    _root = Config::getInstance()->getOutputFolder();
    _main = Utils::getCurrentDateAndTime();
    _params = {"pressure", "density", "temp", "flow", "heatflow"};
    _types = {"data", "pic"};
    _gas = "gas";
}

void ResultsFormatter::writeAll(Mesh* mesh, const std::vector<CellResults*>& results, unsigned int iteration) {
    if (exists(_root) == false) {
        std::cout << "No such folder: " << _root << std::endl;
        return;
    }

    path mainPath{_root / _main};
    if (exists(mainPath) == false) {
        create_directory(mainPath);
    }

//    for (const auto& type : _types) {
//        path typePath{mainPath / type};
//        if (exists(typePath) == false) {
//            create_directory(typePath);
//        }
//    }

    path filePath = mainPath / (std::to_string(iteration) + ".vtk"); // _types[Type::DATA] /
    std::ofstream fs(filePath.generic_string(), std::ios::out); //  | std::ios::binary

    // writing file
    fs << "# vtk DataFile Version 2.0" << std::endl; // header - version and identifier
    fs << "SAMPLE FILE" << std::endl; // title (256c max)
    fs << "ASCII" << std::endl; // data type (ASCII or BINARY)
    fs << "DATASET UNSTRUCTURED_GRID" << std::endl; // type of form
    fs << std::endl;

    // points
    fs << "POINTS " << mesh->getNodes().size() << " " << "double" << std::endl;
    for (const auto& node : mesh->getNodes()) {
        auto point = node->getPosition();
        fs << point.x() << " " << point.y() << " " << point.z() << std::endl;
    }
    fs << std::endl;

    // cells
    std::vector<Element*> elements;
    for (const auto& element : mesh->getElements()) {
        auto pos = std::find_if(results.begin(), results.end(), [&element](CellResults* res) {
            return element->getId() == res->getId();
        });
        if (pos != results.end()) {
            elements.push_back(element.get());
        }
    }
    auto numberOfAllIndices = 0;
    for (auto element : elements) {
        numberOfAllIndices += element->getNodeIds().size();
        numberOfAllIndices += 1;
    }
    fs << "CELLS " << elements.size() << " " << numberOfAllIndices << std::endl;
    for (auto element : elements) {
        const auto& nodeIds = element->getNodeIds();
        fs << nodeIds.size();
        for (const auto& nodeId : nodeIds) {
            fs << " " << (nodeId - 1);
        }
        fs << std::endl;
    }
    fs << std::endl;

    // cell types
    fs << "CELL_TYPES " << elements.size() << std::endl;
    for (auto element : elements) {
        int cellType = 0;
        switch (element->getType()) {
            case Element::Type::POINT:
                cellType = 1;
                break;
            case Element::Type::LINE:
                cellType = 3;
                break;
            case Element::Type::TRIANGLE:
                cellType = 5;
                break;
            case Element::Type::QUADRANGLE:
                cellType = 9;
                break;
            case Element::Type::TETRAHEDRON:
                cellType = 10;
                break;
        }
        fs << cellType << std::endl;
    }
    fs << std::endl;

    // scalar field (density, temperature, pressure) - lookup table "default"
    fs << "CELL_DATA " << elements.size() << std::endl;
    fs << "SCALARS " << "temperature" << " " << "double" << " " << 1 << std::endl;
    fs << "LOOKUP_TABLE " << "default" << std::endl;
    auto normalizer = Config::getInstance()->getNormalizer();
    for (auto element : elements) {
        double value = 0.0;
        auto pos = std::find_if(results.begin(), results.end(), [&element](CellResults* res) {
            return element->getId() == res->getId();
        });
        if (pos != results.end()) {
            value = normalizer->restore((*pos)->getTemp(0), Normalizer::Type::TEMPERATURE);
        }
        fs << value << std::endl;
    }

    fs << "VECTORS " << "flow" << " " << "double" << std::endl;
    for (auto element : elements) {
        Vector3d value;
        auto pos = std::find_if(results.begin(), results.end(), [&element](CellResults* res) {
            return element->getId() == res->getId();
        });
        if (pos != results.end()) {
            value = (*pos)->getFlow(0);
        }
        fs << value.x() << " " << value.y() << " " << value.z() << std::endl;
    }
}

void ResultsFormatter::writeMeshDetails(Mesh* mesh) {
    if (exists(_root) == false) {
        std::cout << "No such folder: " << _root << std::endl;
        return;
    }

    path mainPath{_root / _main};
    if (exists(mainPath) == false) {
        create_directory(mainPath);
    }

    path filePath = mainPath / ("mesh_details.vtk"); // _types[Type::DATA] /
    std::ofstream fs(filePath.generic_string(), std::ios::out); //  | std::ios::binary

    // writing file
    fs << "# vtk DataFile Version 2.0" << std::endl; // header - version and identifier
    fs << "SAMPLE FILE" << std::endl; // title (256c max)
    fs << "ASCII" << std::endl; // data type (ASCII or BINARY)
    fs << "DATASET UNSTRUCTURED_GRID" << std::endl; // type of form
    fs << std::endl;

    // points
    fs << "POINTS " << mesh->getNodes().size() << " " << "double" << std::endl;
    for (const auto& node : mesh->getNodes()) {
        auto point = node->getPosition();
        fs << point.x() << " " << point.y() << " " << point.z() << std::endl;
    }
    fs << std::endl;

    // cells
    std::vector<Element*> elements;
    for (const auto& element : mesh->getElements()) {
        if (element->getType() == Element::Type::TETRAHEDRON) {
            elements.push_back(element.get());
        }
    }
    auto numberOfAllIndices = 0;
    for (auto element : elements) {
        numberOfAllIndices += element->getNodeIds().size();
        numberOfAllIndices += 1;
    }
    fs << "CELLS " << elements.size() << " " << numberOfAllIndices << std::endl;
    for (auto element : elements) {
        const auto& nodeIds = element->getNodeIds();
        fs << nodeIds.size();
        for (const auto& nodeId : nodeIds) {
            fs << " " << (nodeId - 1);
        }
        fs << std::endl;
    }
    fs << std::endl;

    // cell types
    fs << "CELL_TYPES " << elements.size() << std::endl;
    for (auto element : elements) {
        int cellType = 0;
        switch (element->getType()) {
            case Element::Type::POINT:
                cellType = 1;
                break;
            case Element::Type::LINE:
                cellType = 3;
                break;
            case Element::Type::TRIANGLE:
                cellType = 5;
                break;
            case Element::Type::QUADRANGLE:
                cellType = 9;
                break;
            case Element::Type::TETRAHEDRON:
                cellType = 10;
                break;
        }
        fs << cellType << std::endl;
    }
    fs << std::endl;

    // scalar field (density, temperature, pressure) - lookup table "default"
    fs << "CELL_DATA " << elements.size() << std::endl;
    fs << "SCALARS " << "neighbors" << " " << "double" << " " << 1 << std::endl;
    fs << "LOOKUP_TABLE " << "default" << std::endl;
    for (auto element : elements) {
        double value = 0.0;
        for (const auto& sideElement : element->getSideElements()) {
            if (sideElement->getNeighborId() == 0) {
                value += 1.0;
            }
        }
        fs << value << std::endl;
    }

    fs << "VECTORS " << "normals" << " " << "double" << std::endl;
    for (auto element : elements) {
        bool hasBorderSide = false;
        Vector3d value;
        for (const auto& sideElement : element->getSideElements()) {
            if (sideElement->getNeighborId() == 0) {
                hasBorderSide = true;
                value += sideElement->getNormal();
            }
        }
        if (hasBorderSide == false) {
            value = Vector3d(0.0, 0.0, -0.01);
        }
        fs << value.x() << " " << value.y() << " " << value.z() << std::endl;
    }
}
