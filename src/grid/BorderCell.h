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
        FLOW_CONNECT
    };

private:
    std::vector<BorderType> _borderTypes;
    CellParameters _boundaryParams;

    GridBuffer* _gridBuffer;

    // needed for flow connect condition
    std::string _group;
    std::string _groupConnect;

    std::vector<std::vector<double>> _cacheExp;

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

    void setConnectParams(std::string group, std::string groupConnect) {
        _group = std::move(group);
        _groupConnect = std::move(groupConnect);
    }

    void init() override;

    void computeTransfer() override;

    void computeIntegral(int gi0, int gi1) override;

    void computeBetaDecay(int gi0, int gi1, double lambda) override;

    void computeImplicitTransfer(int ii) override;

private:
    void computeTransferDiffuse(unsigned int gi);
    void computeTransferMirror(unsigned int gi);
    void computeTransferPressure(unsigned int gi, double borderPressure);
    void computeTransferFlow(unsigned int gi, double borderFlow);
    double computeTransferFlowConnect(unsigned int gi, double borderFlow);

};


#endif //RGS_BORDERCELL_H
