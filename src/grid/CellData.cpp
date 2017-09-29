#include <utilities/Config.h>
#include "CellData.h"

CellData::CellData() : CellData(Type::NORMAL) {}

CellData::CellData(CellData::Type type) : _type(type), _isProcessing(true), _syncType(SyncType::NONE) {
    Config* config = Config::getInstance();

    unsigned int gasesCount = config->getGasesCount();
    _boundaryTypes.resize(gasesCount);

    _syncAxis.clear();
    for (auto axis : config->getAxis()) {
        _syncAxis.push_back(Utils::asNumber(axis));
    }
}

