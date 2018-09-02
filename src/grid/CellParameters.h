#ifndef RGS_PARAMETERS_H
#define RGS_PARAMETERS_H

#include "utilities/Types.h"
#include "utilities/Config.h"

class CellParameters {
    friend class boost::serialization::access;

private:
    std::vector<double> _pressure;
    std::vector<double> _density;
    std::vector<double> _temp;
    std::vector<Vector3d> _flow;
    std::vector<Vector3d> _heatFlow;

public:
    CellParameters() {
        unsigned int gasesCount = Config::getInstance()->getGasesCount();

        _pressure.resize(gasesCount);
        _density.resize(gasesCount);
        _temp.resize(gasesCount);
        _flow.resize(gasesCount);
        _heatFlow.resize(gasesCount);
    };

    void set(unsigned int gas, double pressure, double density, double temp) {
        _pressure[gas] = pressure;
        _density[gas] = density;
        _temp[gas] = temp;
    }

    void set(unsigned int gas, double pressure, double density, double temp, Vector3d flow, Vector3d heatFlow) {
        set(gas, pressure, density, temp);
        _flow[gas] = std::move(flow);
        _heatFlow[gas] = std::move(heatFlow);
    }

    void reset() {
        std::fill(_pressure.begin(), _pressure.end(), 0.0);
        std::fill(_density.begin(), _density.end(), 0.0);
        std::fill(_temp.begin(), _temp.end(), 0.0);
        std::fill(_flow.begin(), _flow.end(), Vector3d());
        std::fill(_heatFlow.begin(), _heatFlow.end(), Vector3d());
    }

    double getPressure(unsigned int gas) const {
        return _pressure[gas];
    }

    void setPressure(unsigned int gas, double pressure) {
        _pressure[gas] = pressure;
    }

    double getDensity(unsigned int gas) const {
        return _density[gas];
    }

    void setDensity(unsigned int gas, double density) {
        _density[gas] = density;
    }

    double getTemp(unsigned int gas) const {
        return _temp[gas];
    }

    void setTemp(unsigned int gas, double temp) {
        _temp[gas] = temp;
    }

    const Vector3d& getFlow(unsigned int gas) const {
        return _flow[gas];
    }

    void setFlow(unsigned int gas, const Vector3d& flow) {
        _flow[gas] = flow;
    }

    const Vector3d& getHeatFlow(unsigned int gas) const {
        return _heatFlow[gas];
    }

    void setHeatFlow(unsigned int gas, const Vector3d& heatFlow) {
        _heatFlow[gas] = heatFlow;
    }

private:
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & _pressure;
        ar & _density;
        ar & _temp;
        ar & _flow;
        ar & _heatFlow;
    }

};

#endif //RGS_PARAMETERS_H
