#include "ParallelCell.h"
#include "CellConnection.h"
#include "parameters/ImpulseSphere.h"
#include "parameters/Gas.h"
#include "core/Config.h"

ParallelCell::ParallelCell(int id, int recvSyncId, int syncProcessId) : BaseCell(Type::PARALLEL, id) {
    _recvSyncId = recvSyncId;
    _syncProcessId = syncProcessId;
}

void ParallelCell::init() {
    auto config = Config::getInstance();
    const auto& gases = config->getGases();
    const auto& impulses = config->getImpulseSphere()->getImpulses();

    // Allocating space for values and new values
    _values.resize(gases.size());

    for (unsigned int gi = 0; gi < gases.size(); gi++) {
        _values[gi].resize(impulses.size(), 0.0);
    }
}

void ParallelCell::computeTransfer() {
    // nothing
}

void ParallelCell::computeIntegral(unsigned int gi0, unsigned int gi1) {
    // nothing
}

void ParallelCell::computeBetaDecay(unsigned int gi0, unsigned int gi1, double lambda) {
    // nothing
}

int ParallelCell::getRecvSyncId() const {
    return _recvSyncId;
}

int ParallelCell::getSyncProcessId() const {
    return _syncProcessId;
}

std::vector<int> ParallelCell::getSendSyncIds() {
    std::vector<int> sendSyncIds;
    for (const auto& connection : _connections) {
        sendSyncIds.push_back(connection->getSecond()->getId());
    }
    return sendSyncIds;
}
