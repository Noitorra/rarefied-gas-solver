#ifndef RGS_CELL_INFO_H
#define RGS_CELL_INFO_H

#include "utilities/Types.h"
#include "utilities/Utils.h"
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

    // fake parallel related
    int _indexInOriginalGrid;
    int _processorOfOriginalGrid;

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

    int getIndexInOriginalGrid() const {
        return _indexInOriginalGrid;
    }

    void setIndexInOriginalGrid(int indexInOriginalGrid) {
        _indexInOriginalGrid = indexInOriginalGrid;
    }

    int getProcessorOfOriginalGrid() const {
        return _processorOfOriginalGrid;
    }

    void setProcessorOfOriginalGrid(int processorOfOriginalGrid) {
        _processorOfOriginalGrid = processorOfOriginalGrid;
    }

private:
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & _params;
        ar & _boundaryParams;
        ar & _boundaryTypes;
        ar & _step;
        ar & _type;
        ar & _indexInOriginalGrid;
        ar & _processorOfOriginalGrid;
    }
};

#endif //RGS_CELL_INFO_H
