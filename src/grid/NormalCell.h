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
    NormalCell(int id, double volume);

    void init() override;

    void computeTransfer() override;

    void computeIntegral(unsigned int gi0, unsigned int gi1) override;

    void computeBetaDecay(unsigned int gi0, unsigned int gi1, double lambda) override;

    double getVolume() const;

    CellParameters& getParams();

    void swapValues();

    CellResults* getResults();

private:
    double compute_density(unsigned int gi);
    double compute_temperature(unsigned int gi, double density, const Vector3d& stream);
    double compute_pressure(unsigned int gi, double density, double temperature);
    Vector3d compute_stream(unsigned int gi);
    Vector3d compute_heatstream(unsigned int gi);

};

#endif /* RGS_CELL_H */
