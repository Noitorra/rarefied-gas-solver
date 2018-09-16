#ifndef RGS_MESHPARSER_H
#define RGS_MESHPARSER_H

#include "Mesh.h"

#include <map>

class MeshParser {
private:
    enum class Type {
        UNDEFINED,
        MESH_FORMAT,
        PHYSICAL_NAMES,
        NODES,
        ELEMENTS
    };
    std::map<Type, std::string> _keywords;
    Type _type;
    bool _isPhysicalEntitiesSizeLine;
    bool _isNodesSizeLine;
    bool _isElementsSizeLine;

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


#endif //RGS_MESHPARSER_H
