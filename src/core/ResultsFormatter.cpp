#include "ResultsFormatter.h"
#include "utilities/Normalizer.h"
#include "utilities/Utils.h"
#include "grid/Grid.h"
#include "grid/Cell.h"
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

void ResultsFormatter::writeAll(Mesh* mesh, const std::vector<CellResults>& results, unsigned int iteration) {
    if (exists(_root) == false) {
        std::cout << "No such folder: " << _root << std::endl;
        return;
    }

    path mainPath{_root / _main};
    if (exists(mainPath) == false) {
        create_directory(mainPath);
    }

    for (const auto& type : _types) {
        path typePath{mainPath / type};
        if (exists(typePath) == false) {
            create_directory(typePath);
        }
    }

    path filePath = mainPath / _types[Type::DATA] / (std::to_string(iteration) + ".vtk");
    std::ofstream fs(filePath.generic_string(), std::ios::out); //  | std::ios::binary

    // writing file
    fs << "# vtk DataFile Version 3.0" << std::endl; // header - version and identifier
    fs << "Result for mesh (NAME)" << std::endl; // title (256c max)
    fs << "ASCII" << std::endl; // data type (ASCII or BINARY)
    fs << "DATASET UNSTRUCTURED_GRID" << std::endl; // type of form

    // points
    fs << "POINTS " << mesh->getNodesCount() << "double" << std::endl;
    for (auto i = 0; i < mesh->getNodesCount(); i++) {
        auto point = mesh->getNode(i)->getPosition();
        fs << point.x() << " " << point.y() << " " << point.z() << std::endl;
    }

    // cells
    std::vector<Element*> elements;
    for (const auto& element : mesh->getElements()) {
        auto pos = std::find_if(results.begin(), results.end(), [&element](const CellResults& res) {
            return element->getId() == res.getId();
        });
        if (pos != results.end()) {
            elements.push_back(element.get());
        }
    }
    auto numberOfAllIndices = 0;
    for (auto element : elements) {
        numberOfAllIndices += element->getNodes().size();
    }
    fs << "CELLS " << elements.size() << " " << numberOfAllIndices << std::endl;
    for (auto element : elements) {
        const auto& nodes = element->getNodes();
        fs << nodes.size();
        for (const auto& node : nodes) {
            fs << " " << node->getId();
        }
        fs << std::endl;
    }

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
        }
        fs << cellType << std::endl;
    }

    // scalar field (density, temperature, pressure) - lookup table "default"
    fs << "CELL_DATA " << elements.size();
    fs << "SCALARS " << "density" << " " << "double" << " " << 1 << std::endl;
    fs << "LOOKUP_TABLE " << "default" << std::endl;
    auto normalizer = Config::getInstance()->getNormalizer();
    for (auto element : elements) {
        double value = 0.0;
        auto pos = std::find_if(results.begin(), results.end(), [&element](const CellResults& res) {
            return element->getId() == res.getId();
        });
        if (pos != results.end()) {
            value = normalizer->restore(pos->getPressure(0), Normalizer::Type::PRESSURE);
        }
        fs << value << std::endl;
    }

//        for (unsigned int gi = 0; gi < Config::getInstance()->getGasesCount(); gi++) {
//          for (unsigned int paramIndex = Param::PRESSURE; paramIndex <= Param::HEATFLOW; paramIndex++) {
//            std::vector<double> writeBuffer;
//            for (unsigned int y = 0; y < grid->getSize().y(); y++) {
//                for (unsigned int x = 0; x < grid->getSize().x(); x++) {
//                    writeBuffer.clear();
//
//                    auto* params = grid->get(x, y);
//                    if (params != nullptr) {
//                        switch (paramIndex) {
//                            case Param::PRESSURE:
//                                writeBuffer.push_back(normalizer->restore(params->getPressure(gi), Normalizer::Type::PRESSURE));
//                                break;
//                            case Param::DENSITY:
//                                writeBuffer.push_back(normalizer->restore(params->getDensity(gi), Normalizer::Type::DENSITY));
//                                break;
//                            case Param::TEMPERATURE:
//                                writeBuffer.push_back(normalizer->restore(params->getTemp(gi), Normalizer::Type::TEMPERATURE));
//                                break;
//                            case Param::FLOW:
//                                writeBuffer.push_back(normalizer->restore(params->getFlow(gi).x(), Normalizer::Type::FLOW));
//                                writeBuffer.push_back(normalizer->restore(params->getFlow(gi).y(), Normalizer::Type::FLOW));
//                                break;
//                            case Param::HEATFLOW: // TODO: add heat flow normalizer
////                                normalizer->restore(heatflow.x(), Normalizer::Type::FLOW);
////                                normalizer->restore(heatflow.y(), Normalizer::Type::FLOW);
//                                writeBuffer.push_back(params->getHeatFlow(gi).x());
//                                writeBuffer.push_back(params->getHeatFlow(gi).y());
//                                break;
//                            default:
//                                break;
//                        }
//                    } else {
//                        switch (paramIndex) {
//                            case Param::PRESSURE: case Param::DENSITY: case Param::TEMPERATURE:
//                                writeBuffer.push_back(0.0);
//                                break;
//                            case Param::FLOW: case Param::HEATFLOW:
//                                writeBuffer.push_back(0.0);
//                                writeBuffer.push_back(0.0);
//                                break;
//                            default:
//                                break;
//                        }
//                    }
//
//                    fs.write(reinterpret_cast<const char*>(writeBuffer.data()), sizeof(double) * writeBuffer.size());
//                }
//            }
//        }
//    }
}