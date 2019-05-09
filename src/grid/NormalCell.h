#ifndef RGS_CELL_H
#define RGS_CELL_H

#include "BaseCell.h"
#include "CellParameters.h"
#include "CellResults.h"

class NormalCell : public BaseCell {
private:
    double _volume;
    CellParameters _params;
    std::vector<std::vector<double>> _newValues;
    std::shared_ptr<CellResults> _results;

public:
    NormalCell(int id, double volume) : BaseCell(Type::NORMAL, id) {
        _volume = volume;
    }

    double getVolume() const {
        return _volume;
    }

    CellParameters& getParams() {
        return _params;
    }

    void init() override;

    void computeTransfer() override;

    void computeIntegral(int gi0, int gi1) override;

    void computeBetaDecay(int gi0, int gi1, double lambda) override;

    void swapValues();

    void computeImplicitTransfer(int ii) override;

    CellResults* getResults();

private:
    double compute_density(int gi);
    double compute_temperature(int gi, double density, const Vector3d& stream);
    double compute_pressure(int gi, double density, double temperature);
    Vector3d compute_stream(int gi);
    Vector3d compute_heatstream(int gi);

};

#endif /* RGS_CELL_H */
