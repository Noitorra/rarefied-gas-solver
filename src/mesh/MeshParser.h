//
// Created by Dmitry Sherbakov on 12.08.2018.
//

#ifndef TESTING_MESHPARSER_H
#define TESTING_MESHPARSER_H

#include <map>
#include "Mesh.h"

class MeshParser {
private:
    enum class Type {
        UNDEFINED,
        MESH_FORMAT,
        NODES,
        ELEMENTS
    };
    std::map<Type, std::string> _keywords;
    Type _type;
    Mesh* _mesh;
    float _version;
    int _dataType;
    int _fileSize;

public:
    static MeshParser& getInstance() {
        static MeshParser instance;
        return instance;
    }
    MeshParser(MeshParser const&) = delete;
    MeshParser& operator=(MeshParser const&) = delete;

    Mesh *loadMesh(const std::string& filename);

private:
    MeshParser();
    ~MeshParser() = default;

    void parse(const std::string& line);

};


#endif //TESTING_MESHPARSER_H
