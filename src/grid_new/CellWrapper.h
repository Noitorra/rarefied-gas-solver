#ifndef RGS_CELL_INFO_H
#define RGS_CELL_INFO_H

#include "utilities/types.h"
#include "utilities/utils.h"
#include "Parameters.h"
#include "Cell.h"

class CellWrapper {
    friend class boost::serialization::access;

public:
    enum class BoundaryCondition {
        DIFFUSE,
        PRESSURE,
        FLOW,
        MIRROR
    };
    enum class Type {
        NORMAL,
        SPECIAL
    };

private:
    std::vector<Parameters> _params;
    std::vector<Parameters> _boundaryParams;
    std::vector<BoundaryCondition> _boundaryConditions;
    Vector3d _step;
    Type _type;
//    Cell* _cell;

public:
    CellWrapper() {}

    CellWrapper(unsigned int gasesCount) : _type(Type::NORMAL) {
        _params.resize(gasesCount);
        _boundaryParams.resize(gasesCount);
    }

    void init() {
//        _cell = new Cell(this);
//        _cell->init(_params, _step, -1);
    }

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

    bool isSpecial() const {
        return _type == Type::SPECIAL;
    }

    const std::vector<BoundaryCondition>& getBoundaryConditions() const {
        return _boundaryConditions;
    }

    void setBoundaryConditions(const std::vector<BoundaryCondition>& boundaryConditions) {
        _boundaryConditions = boundaryConditions;
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
