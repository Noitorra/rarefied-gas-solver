#include "MeshParser.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

using namespace std;

MeshParser::MeshParser() {
    _type = Type::UNDEFINED;

    _keywords[Type::MESH_FORMAT] = "MeshFormat";
    _keywords[Type::PHYSICAL_NAMES] = "PhysicalNames";
    _keywords[Type::NODES] =  "Nodes";
    _keywords[Type::ELEMENTS] = "Elements";

    _level = 0;

    _mesh = nullptr;

    _majorVersion = 0;
    _dataType = 0;
    _fileSize = 0;
}

Mesh *MeshParser::loadMesh(const string &filename, double units) {
    std::ifstream fs(filename);
    if (fs.is_open() == true) {
        _mesh = new Mesh();

        _majorVersion = 0;
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

            cout << "Successful mesh parsing: version = " << _version << "; type = " << _dataType << "; size = " << _fileSize << endl;
        } catch (std::exception& e) {
            delete _mesh;
            _mesh = nullptr;

            throw std::runtime_error(std::string("parsing error: ") + e.what());
        }
    }
    return _mesh;
}

void MeshParser::parse(const string& line, double units) {
    if (line.empty() == false) {
        if (line[0] == '$') { // it is directive
            if (_type == Type::UNDEFINED) {
                for (auto const& keyword : _keywords) {
                    if (line.find(keyword.second) != string::npos) {
                        _type = keyword.first;
                        _level = 0;
                        break;
                    }
                }
            } else {
                if (line.find(_keywords[_type]) != string::npos && line.find("End") != string::npos) {
                    _type = Type::UNDEFINED;
                } else {
                    throw runtime_error("wrong directive order");
                }
            }
        } else {
            if (_majorVersion == 0) {
                parseVersion(line);
            } else if (_majorVersion == 2) {
                parseDataV2(line, units);
            }
        }
    }
}

void MeshParser::parseVersion(const std::string& line) {
    std::istringstream is(line);
    if (_type == Type::MESH_FORMAT) {
        is >> _version >> _dataType >> _fileSize;
        _majorVersion = stoi(_version.substr(0, _version.find('.')));
    }
}

void MeshParser::parseDataV2(const string& line, double units) {
    std::istringstream is(line);
    switch (_type) {
        case Type::PHYSICAL_NAMES:
            if (_level == 0) {
                size_t capacity;
                is >> capacity;
                _mesh->reservePhysicalEntities(capacity);

                _level++;
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
        case Type::NODES:
            if (_level == 0) {
                size_t capacity;
                is >> capacity;
                _mesh->reserveNodes(capacity);

                _level++;
            } else if (_level == 1) {
                int id;
                double x, y, z;
                is >> id >> x >> y >> z;
                x *= units;
                y *= units;
                z *= units;
                _mesh->addNode(id, Vector3d(x, y, z));
            }
            break;
        case Type::ELEMENTS:
            if (_level == 0) {
                size_t capacity;
                is >> capacity;
                _mesh->reserveElements(capacity);

                _level++;
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
                for (auto i = 3; i < tags.size(); i++) {
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

