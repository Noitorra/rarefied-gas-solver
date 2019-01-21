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
    std::vector<std::vector<double>> _cacheExp;

public:
    explicit BorderCell(int id) : BaseCell(Type::BORDER, id) {
        const auto& gases = Config::getInstance()->getGases();
        _borderTypes.resize(gases.size(), BorderType::UNDEFINED);
    }

    void setBorderType(int gi, BorderCell::BorderType borderType) {
        _borderTypes[gi] = borderType;
    }

    CellParameters& getBoundaryParams() {
        return _boundaryParams;
    }

    void init() override;

    void computeTransfer() override;

    void computeIntegral(int gi0, int gi1) override;

    void computeBetaDecay(int gi0, int gi1, double lambda) override;

private:
    void computeTransferDiffuse(unsigned int gi);
    void computeTransferMirror(unsigned int gi);
    void computeTransferPressure(unsigned int gi);
    void computeTransferFlow(unsigned int gi);

};


#endif //RGS_BORDERCELL_H
