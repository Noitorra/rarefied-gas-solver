#ifndef RGS_BORDERCELL_H
#define RGS_BORDERCELL_H

#include "BaseCell.h"
#include "CellParameters.h"

class BorderCell : public BaseCell {
public:
    enum class BorderType {
        UNDEFINED,
        DIFFUSE,
        PRESSURE,
        FLOW,
        MIRROR
    };

private:
    std::vector<BorderType> _borderTypes;
    CellParameters _boundaryParams;

public:
    explicit BorderCell(int id);

    void init() override;

    void computeTransfer() override;

    void computeIntegral(int gi0, int gi1) override;

    void computeBetaDecay(int gi0, int gi1, double lambda) override;

    void setBorderType(int id, BorderType borderType);

    CellParameters& getBoundaryParams();

private:
    void computeTransferDiffuse(unsigned int gi);
    void computeTransferMirror(unsigned int gi);
    void computeTransferPressure(unsigned int gi);
    void computeTransferFlow(unsigned int gi);

};


#endif //RGS_BORDERCELL_H
