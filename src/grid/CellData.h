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
        FAKE
    };
    enum class SyncType {
        NONE,
        HALF_VALUES,
        VALUES,
        ALL
    };

private:
    CellParameters _params;
    CellParameters _boundaryParams;
    std::vector<BoundaryType> _boundaryTypes;
    Vector3d _step;
    Type _type;

    int _id; // unique identifier for detecting cells
    bool _isProcessing; // flag to disable fake parallel cells processing

    int _syncId; // id to which this cell is synced
    int _syncProcessorRank; // processor rank to which this cell is synced
    std::vector<int> _syncAxis; // axis on which this cell do sync
    SyncType _syncType; // what part of cell data we need to sync

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

    bool isProcessing() const {
        return _isProcessing;
    }

    void setProcessing(bool isProcessing) {
        _isProcessing = isProcessing;
    }

    SyncType getSyncType() const {
        return _syncType;
    }

    void setSyncType(SyncType syncType) {
        _syncType = syncType;
    }

    bool isSyncHalfValues() {
        return _syncType == SyncType::HALF_VALUES || _syncType == SyncType::ALL;
    }

    bool isSyncValues() {
        return _syncType == SyncType::VALUES || _syncType == SyncType::ALL;
    }

    int getId() const {
        return _id;
    }

    void setId(int id) {
        _id = id;
    }

    int getSyncId() const {
        return _syncId;
    }

    void setSyncId(int syncId) {
        _syncId = syncId;
    }

    int getSyncProcessorRank() const {
        return _syncProcessorRank;
    }

    void setSyncProcessorRank(int syncProcessorRank) {
        _syncProcessorRank = syncProcessorRank;
    }

    bool isSyncAvaliable(int axis) const {
        return std::find(_syncAxis.begin(), _syncAxis.end(), axis) != _syncAxis.end();
    }

    void addSyncAxis(int axis) {
        _syncAxis.push_back(axis);
    }

    void setSyncAxis(int axis) {
        _syncAxis.clear();
        _syncAxis.push_back(axis);
    }

    void removeSyncAxis() {
        _syncAxis.clear();
    }

private:
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & _params;
        ar & _boundaryParams;
        ar & _boundaryTypes;
        ar & _step;
        ar & _type;
        ar & _id;
        ar & _isProcessing;
        ar & _syncId;
        ar & _syncProcessorRank;
        ar & _syncAxis;
        ar & _syncType;
    }
};

#endif //RGS_CELL_INFO_H
