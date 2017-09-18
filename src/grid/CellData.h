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
    CellParameters _params;
    CellParameters _boundaryParams;
    std::vector<BoundaryType> _boundaryTypes;
    Vector3d _step;
    Type _type;

public:
    CellData();

    explicit CellData(Type type);

    CellData(const CellData& o) = default;

    CellParameters& params() {
        return _params;
    }

    CellParameters& boundaryParams() {
        return _boundaryParams;
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

    bool isNormal() const {
        return _type == Type::NORMAL;
    }

    bool isFake() const {
        return _type == Type::FAKE;
    }

    bool isFakeParallel() const {
        return _type == Type::FAKE_PARALLEL;
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
