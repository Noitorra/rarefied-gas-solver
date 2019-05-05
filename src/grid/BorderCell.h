#ifndef RGS_BORDERCELL_H
#define RGS_BORDERCELL_H

#include "BaseCell.h"
#include "CellParameters.h"
#include "GridBuffer.h"

class BorderCell : public BaseCell {
public:
    enum class BorderType {
        UNDEFINED,
        DIFFUSE,
        PRESSURE,
        MIRROR,
        FLOW,
        PRESSURE_FROM,
        PRESSURE_TO
    };

private:
    std::vector<BorderType> _borderTypes;
    CellParameters _boundaryParams;
    std::vector<std::vector<double>> _cacheExp;
    GridBuffer* _gridBuffer;

public:
    explicit BorderCell(int id, GridBuffer* gridBuffer) : BaseCell(Type::BORDER, id), _gridBuffer(gridBuffer) {
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

    void storeResults();

private:
    void computeTransferDiffuse(unsigned int gi);
    void computeTransferMirror(unsigned int gi);
    void computeTransferPressure(unsigned int gi, double borderPressure);
    void computeTransferFlow(unsigned int gi, const Vector3d& borderFlow);

    void storeResults(unsigned int gi);

};


#endif //RGS_BORDERCELL_H
