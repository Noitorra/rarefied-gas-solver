#ifndef RGS_CELL_INFO_H
#define RGS_CELL_INFO_H

#include "utilities/types.h"
#include "utilities/utils.h"
#include "Parameters.h"
#include "Cell.h"

class CellData {
    friend class boost::serialization::access;

public:
    enum class BoundaryType {
        DIFFUSE,
        PRESSURE,
        FLOW,
        MIRROR
    };
    enum class Type {
        NORMAL,
        FAKE,
        FAKE_PARALLEL
    };

private:
    std::vector<Parameters> _params;
    std::vector<Parameters> _boundaryParams;
    std::vector<BoundaryType> _boundaryTypes;
    Vector3d _step;
    Type _type;

public:
    CellData() : _type(Type::NORMAL) {}

    CellData(Type type, unsigned int gasesCount) : _type(type) {
        _params.resize(gasesCount);
        _boundaryParams.resize(gasesCount);
    }

    CellData(const CellData& o) = default;

    Parameters& getParams(unsigned int gas) {
        return _params[gas];
    }

    Parameters& getBoundaryParams(unsigned int gas) {
        return _boundaryParams[gas];
    }

    const Vector3d &getStep() const {
        return _step;
    }

    void setStep(const Vector3d &step) {
        _step = step;
    }

    Type getType() const {
        return _type;
    }

    void setType(Type type) {
        _type = type;
    }

    const std::vector<BoundaryType>& getBoundaryTypes() const {
        return _boundaryTypes;
    }

    void setBoundaryTypes(const std::vector<BoundaryType>& boundaryConditions) {
        _boundaryTypes = boundaryConditions;
    }

private:
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & _params;
        ar & _boundaryParams;
        ar & _step;
        ar & _type;
    }
};

#endif //RGS_CELL_INFO_H
