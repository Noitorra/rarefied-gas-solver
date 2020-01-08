#ifndef RGS_MESHPARSER_H
#define RGS_MESHPARSER_H

#include "Mesh.h"

#include <map>
#include <vector>

class MeshParser {
private:
    enum class Directives {
        UNDEFINED,
        MESH_FORMAT,
        PHYSICAL_NAMES,
        NODES,
        ELEMENTS,

        // for v4
        ENTITIES,
        PARTITIONED_ENTITIES
    };

    struct Entity {
        int tag;
        std::vector<int> physicalTags;
    };

    struct PointEntity : Entity {
        double x, y, z;
    };

    struct CurveEntity : Entity {
        double minX, minY, minZ, maxX, maxY, maxZ;
        std::vector<int> boundingPoints;
    };

    struct SurfaceEntity : Entity {
        double minX, minY, minZ, maxX, maxY, maxZ;
        std::vector<int> boundingCurves;
    };

    struct VolumeEntity : Entity {
        double minX, minY, minZ, maxX, maxY, maxZ;
        std::vector<int> boundingSurfaces;
    };

    std::map<Directives, std::string> _keywords;

    // used recording parsing state
    Directives _directive;
    int _level;

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

    Mesh* loadMesh(const std::string& filename, double units);

private:
    MeshParser();
    ~MeshParser() = default;

    void parse(const std::string& line, double units);
    void parseDataV2(std::istringstream& is, double units);
    void parseDataV4(std::istringstream& is, double units);
};

#endif //RGS_MESHPARSER_H
