#include <utilities/Config.h>
#include <boost/filesystem.hpp>
#include <iostream>
#include "ResultsWriter.h"
#include "grid/Grid.h"
#include "grid/Cell.h"
#include "utilities/Normalizer.h"

using namespace boost::filesystem;

ResultsWriter::ResultsWriter() {
    _root = Config::getInstance()->getOutputFolder();
    _main = "123"; // TODO: Generate timecode for main folder name
    _params = {"pressure", "density", "temp", "flow", "heatflow"};
    _types = {"data", "pic"};
    _gas = "gas";
}

void ResultsWriter::writeAll(Grid<CellParameters>* grid, unsigned int iteration) {
    if (exists(_root) == false) {
        std::cout << "No such folder: " << _root << std::endl;
        return;
    }

    path mainPath{_root / _main};
    if (exists(mainPath) == false) {
        create_directory(mainPath);
    }

    // TODO: write grid to files
    for (unsigned int gi = 0; gi < Config::getInstance()->getGasesCount(); gi++) {
        path gasPath{mainPath / (_gas + std::to_string(gi))};
        if (exists(gasPath) == false) {
            create_directory(gasPath);
        }

        for (unsigned int paramIndex = Param::PRESSURE; paramIndex <= Param::HEATFLOW; paramIndex++) {
            const auto& param = _params[paramIndex];

            path paramPath{gasPath / param};
            if (exists(paramPath) == false) {
                create_directory(paramPath);
            }


            for (const auto& type : _types) {
                path typePath{paramPath / type};
                if (exists(typePath) == false) {
                    create_directory(typePath);
                }
            }

            path filePath = paramPath / _types[Type::DATA] / (std::to_string(iteration) + ".bin");
            std::ofstream fs(filePath.generic_string(), std::ios::out | std::ios::binary);

            auto tmp = static_cast<double>(grid->getSize().x());
            fs.write(reinterpret_cast<const char*>(&tmp), sizeof(double));
            tmp = static_cast<double>(grid->getSize().y());
            fs.write(reinterpret_cast<const char*>(&tmp), sizeof(double));

            const Normalizer* normalizer = Config::getInstance()->getNormalizer();
            std::vector<double> writeBuffer;

            for (unsigned int y = 0; y < grid->getSize().y(); y++) {
                for (unsigned int x = 0; x < grid->getSize().x(); x++) {
                    writeBuffer.clear();

                    auto* params = grid->get(x, y);
                    if (params != nullptr) {
                        switch (paramIndex) {
                            case Param::PRESSURE:
                                writeBuffer.push_back(normalizer->restore(params->getPressure(gi), Normalizer::Type::PRESSURE));
                                break;
                            case Param::DENSITY:
                                writeBuffer.push_back(normalizer->restore(params->getDensity(gi), Normalizer::Type::DENSITY));
                                break;
                            case Param::TEMPERATURE:
                                writeBuffer.push_back(normalizer->restore(params->getTemp(gi), Normalizer::Type::TEMPERATURE));
                                break;
                            case Param::FLOW:
                                writeBuffer.push_back(normalizer->restore(params->getFlow(gi).x(), Normalizer::Type::FLOW));
                                writeBuffer.push_back(normalizer->restore(params->getFlow(gi).y(), Normalizer::Type::FLOW));
                                break;
                            case Param::HEATFLOW: // TODO: add heat flow normalizer
//                                normalizer->restore(heatflow.x(), Normalizer::Type::FLOW);
//                                normalizer->restore(heatflow.y(), Normalizer::Type::FLOW);
                                writeBuffer.push_back(params->getHeatFlow(gi).x());
                                writeBuffer.push_back(params->getHeatFlow(gi).y());
                                break;
                            default:
                                break;
                        }
                    } else {
                        switch (paramIndex) {
                            case Param::PRESSURE: case Param::DENSITY: case Param::TEMPERATURE:
                                writeBuffer.push_back(0.0);
                                break;
                            case Param::FLOW: case Param::HEATFLOW:
                                writeBuffer.push_back(0.0);
                                writeBuffer.push_back(0.0);
                                break;
                            default:
                                break;
                        }
                    }

                    fs.write(reinterpret_cast<const char*>(writeBuffer.data()), sizeof(double) * writeBuffer.size());
                }
            }
        }
    }
}