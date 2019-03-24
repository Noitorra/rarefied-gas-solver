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
    int _level;

    Mesh* _mesh;
    std::string _version;
    int _majorVersion;
    int _dataType;
    int _fileSize;

public:
    static MeshParser& getInstance() {
        static MeshParser instance;
        return instance;
    }
    MeshParser(MeshParser const&) = delete;
    MeshParser& operator=(MeshParser const&) = delete;

    Mesh *loadMesh(const std::string& filename, double units);

private:
    MeshParser();
    ~MeshParser() = default;

    void parse(const std::string& line, double units);
    void parseVersion(const std::string& line);
    void parseDataV2(const std::string& line, double units);

};


#endif //RGS_MESHPARSER_H
