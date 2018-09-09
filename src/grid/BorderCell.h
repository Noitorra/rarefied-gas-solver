#ifndef RGS_BORDERCELL_H
#define RGS_BORDERCELL_H

#include "BaseCell.h"
#include "CellParameters.h"

class BorderCell : public BaseCell {
public:
    enum class BorderType {
        DIFFUSE,
        PRESSURE,
        FLOW,
        MIRROR
    };

private:
    BorderType _borderType;
    CellParameters _boundaryParams;

public:
    explicit BorderCell(BorderType borderType);

    void init() override;

    void computeTransfer() override;

    void computeIntegral(unsigned int gi0, unsigned int gi1) override;

    void computeBetaDecay(unsigned int gi0, unsigned int gi1, double lambda) override;

    CellParameters& getBoundaryParams();

};


#endif //RGS_BORDERCELL_H
