#include <utilities/config.h>
#include "CellData.h"

CellData::CellData() : CellData(Type::NORMAL) {}

CellData::CellData(CellData::Type type) : _type(type) {
    unsigned int gasesCount = Config::getInstance()->getGasesCount();

    _boundaryTypes.resize(gasesCount);
}

