#ifndef RGS_CELL_INFO_H
#define RGS_CELL_INFO_H

#include "utilities/types.h"
#include "utilities/utils.h"
#include "CellParameters.h"
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
    std::vector<CellParameters> _params;
    std::vector<CellParameters> _boundaryParams;
    std::vector<BoundaryType> _boundaryTypes;
    Vector3d _step;
    Type _type;

public:
    CellData();

    explicit CellData(Type type);

    CellData(const CellData& o) = default;

    CellParameters& params(unsigned int gas) {
        return _params[gas];
    }

    CellParameters& boundaryParams(unsigned int gas) {
        return _boundaryParams[gas];
    }

    const BoundaryType getBoundaryType(unsigned int gas) const {
        return _boundaryTypes[gas];
    }

    void setBoundaryTypes(unsigned int gas, BoundaryType boundaryType) {
        _boundaryTypes[gas] = boundaryType;
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
