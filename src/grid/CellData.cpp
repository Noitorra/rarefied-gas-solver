#include <utilities/config.h>
#include "CellData.h"

CellData::CellData() : CellData(Type::NORMAL) {}

CellData::CellData(CellData::Type type) : _type(type) {
    unsigned int gasesCount = Config::getInstance()->getGasesCount();

    _params.resize(gasesCount);
    _boundaryParams.resize(gasesCount);
    _boundaryTypes.resize(gasesCount);
}

