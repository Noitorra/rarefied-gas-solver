#ifndef RGS_PARALLELCELL_H
#define RGS_PARALLELCELL_H

#include "BaseCell.h"

class ParallelCell : public BaseCell {
private:
    int _recvSyncId;
    int _syncProcessId;

public:
    ParallelCell(int id, int recvSyncId, int syncProcessId);

    void init() override;

    void computeTransfer() override;

    void computeIntegral(int gi0, int gi1) override;

    void computeBetaDecay(int gi0, int gi1, double lambda) override;

    int getRecvSyncId() const;

    int getSyncProcessId() const;

    std::vector<int> getSendSyncIds();
};


#endif //RGS_PARALLELCELL_H
