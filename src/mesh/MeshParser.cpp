#include "MeshParser.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

using namespace std;

MeshParser::MeshParser() {
    _keywords = {
            {Directives::MESH_FORMAT,    "MeshFormat"},
            {Directives::PHYSICAL_NAMES, "PhysicalNames"},
            {Directives::NODES,          "Nodes"},
            {Directives::ELEMENTS,       "Elements"}
    };

    _directive = Directives::UNDEFINED;
    _level = 0;

    _mesh = nullptr;
    _version = 0.0;
    _dataType = 0;
    _fileSize = 0;
}

Mesh *MeshParser::loadMesh(const string &filename, double units) {
    std::ifstream fs(filename);
    if (fs.is_open()) {
        _mesh = new Mesh();

        _version = 0.0;
        _dataType = 0;
        _fileSize = 0;

        try {
            string line;
            while (getline(fs, line)) {
                if (line.empty() == false && *line.rbegin() == '\r') {
                    line = line.substr(0, line.size() - 1);
                }
                parse(line, units);
            }
            fs.close();

            cout << "Successful mesh parsing: "
                 << "version = " << _version << "; "
                 << "type = " << _dataType << "; "
                 << "size = " << _fileSize << "; "
                 << "number_of_elements = " << _mesh->getElements().size() << endl << endl;
        } catch (std::exception& e) {
            delete _mesh;
            _mesh = nullptr;

            throw std::runtime_error(string("mesh parsing error: ") + e.what());
        }
    } else {
        throw std::runtime_error("mesh file not found: " + filename);
    }
    return _mesh;
}

void MeshParser::parse(const string& line, double units) {
    if (line.empty() == false) {
        if (line[0] == '$') { // it is directive
            if (_directive == Directives::UNDEFINED) {
                for (auto const& keyword : _keywords) {
                    if (line.find(keyword.second) != string::npos) {
                        _directive = keyword.first;
                        _level = 0;
                        break;
                    }
                }
            } else {
                if (line.find(_keywords[_directive]) != string::npos && line.find("End") != string::npos) {
                    _directive = Directives::UNDEFINED;
                } else {
                    throw runtime_error("wrong directive order");
                }
            }
        } else {
            std::istringstream is(line);
            if (_directive == Directives::MESH_FORMAT) {
                is >> _version >> _dataType >> _fileSize;
            } else if (int(_version) == 2) {
                parseDataV2(is, units);
            }
        }
    }
}

void MeshParser::parseDataV2(std::istringstream& is, double units) {
    switch (_directive) {
        case Directives::PHYSICAL_NAMES:
            if (_level++ == 0) {
                size_t capacity;
                is >> capacity;
                _mesh->reservePhysicalEntities(capacity);
            } else {
                int dimension, tag;
                std::string name;
                is >> dimension >> tag >> name;
                if (name[0] == '\"') {
                    name.erase(0, 1);
                    name.erase(name.size() - 1);
                }
                _mesh->addPhysicalEntity(dimension, tag, name);
            }
            break;
        case Directives::NODES:
            if (_level++ == 0) {
                size_t capacity;
                is >> capacity;
                _mesh->reserveNodes(capacity);
            } else {
                int id;
                double x, y, z;
                is >> id >> x >> y >> z;
                x *= units;
                y *= units;
                z *= units;
                _mesh->addNode(id, Vector3d(x, y, z));
            }
            break;
        case Directives::ELEMENTS:
            if (_level++ == 0) {
                size_t capacity;
                is >> capacity;
                _mesh->reserveElements(capacity);
            } else {
                int id, type, tagsCount;
                is >> id >> type >> tagsCount;
                vector<int> tags;
                for (int i = 0; i < tagsCount; i++) {
                    int tag;
                    is >> tag;
                    tags.push_back(tag);
                }
                int physicalEntityId = tags[0];
                int geomUnitId = tags[1];
                vector<int> partitions;
                for (size_t i = 3; i < tags.size(); i++) {
                    partitions.push_back(tags[i]);
                }

                vector<int> nodeIds;
                do {
                    int nodeId;
                    is >> nodeId;
                    nodeIds.push_back(nodeId);
                } while (is.good());

                _mesh->addElement(id, type, physicalEntityId, geomUnitId, partitions, nodeIds);
            }
            break;
        default:
            break;
    }
}

void MeshParser::parseDataV4(std::istringstream& is, double units) {
    switch(_directive) {
        case Directives::PHYSICAL_NAMES:
            if (_level++ == 0) {
                size_t capacity;
                is >> capacity;
                _mesh->reservePhysicalEntities(capacity);
            } else {
                int dimension, tag;
                std::string name;
                is >> dimension >> tag >> name;
                if (name[0] == '\"') {
                    name.erase(0, 1);
                    name.erase(name.size() - 1);
                }
                _mesh->addPhysicalEntity(dimension, tag, name);
            }
            break;
        case Directives::ENTITIES:
            if (_level++ == 0) {
                int numPoints, numCurves, numSurfaces, numVolumes;
                is >> numPoints >> numCurves >> numSurfaces >> numVolumes;
            } else {
                // need to iterate over many points
                // then over curves, surfaces, volumes

                int tag;
                double x, y, z;
                size_t numPhysicalTags;
                vector<int> physicalTags;
                is >> tag >> x >> y >> z >> numPhysicalTags;
                for (size_t i = 0; i < numPhysicalTags; i++) {
                    int physicalTag;
                    is >> physicalTag;
                    physicalTags.push_back(physicalTag);
                }
            }
            break;
        case Directives::PARTITIONED_ENTITIES:
            break;
    }
}

