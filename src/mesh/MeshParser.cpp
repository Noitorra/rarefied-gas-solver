//
// Created by Dmitry Sherbakov on 12.08.2018.
//

#include <fstream>
#include <sstream>
#include "MeshParser.h"
#include "Elements.h"

using namespace std;

MeshParser::MeshParser() {
    _type = Type::UNDEFINED;

    _keywords.emplace(Type::MESH_FORMAT, "MeshFormat");
    _keywords.emplace(Type::NODES, "Nodes");
    _keywords.emplace(Type::ELEMENTS, "Elements");

    _mesh = nullptr;
}

Mesh *MeshParser::loadMesh(const string &filename) {
    std::ifstream fs(filename);
    if (fs.is_open() == true) {
        _mesh = new Mesh();

        try {
            string line;
            while (getline(fs, line)) {
                line = line.substr(0, line.size() - 1);
                parse(line);
            }
            fs.close();

            cout << "Successful mesh parsing: version = " << _version << "; type = " << _dataType << "; size = " << _fileSize << endl;
        } catch (std::exception& e) {
            delete _mesh;
            _mesh = nullptr;

            cout << "Errors during mesh parsing - " << e.what() << endl;
        }
    }
    return _mesh;
}

void MeshParser::parse(const string& line) {
    if (line.empty() == false) {
        if (line[0] == '$') { // it is directive
            if (_type == Type::UNDEFINED) {
                for (auto const& keyword : _keywords) {
                    if (line.find(keyword.second) != string::npos) {
                        _type = keyword.first;
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
        } else { // it is usual data
            std::istringstream is(line);
            switch (_type) {
                case Type::UNDEFINED:
                    throw runtime_error("missing directive");
                case Type::MESH_FORMAT:
                    is >> _version >> _dataType >> _fileSize;
                    break;
                case Type::NODES:
                    if (_mesh->getNodesCapacity() == 0) {
                        size_t capacity;
                        is >> capacity;
                        _mesh->setNodesCapacity(capacity);
                    } else {
                        int id;
                        double x, y, z;
                        is >> id >> x >> y >> z;
                        _mesh->addNode(id, Vector3d(x, y, z));
                    }
                    break;
                case Type::ELEMENTS:
                    if (_mesh->getElementsCapacity() == 0) {
                        size_t capacity;
                        is >> capacity;
                        _mesh->setElementsCapacity(capacity);
                    } else {
                        int id, type, tagsCount;
                        is >> id >> type >> tagsCount;
                        vector<int> tags;
                        for (int i = 0; i < tagsCount; i++) {
                            int tag;
                            is >> tag;
                            tags.push_back(tag);
                        }
                        vector<int> nodeIds;
                        do {
                            int nodeId;
                            is >> nodeId;
                            nodeIds.push_back(nodeId);
                        } while (is.good());
                        _mesh->addElement(id, type, tags, nodeIds);
                    }
                    break;
            }
        }
    }
}


