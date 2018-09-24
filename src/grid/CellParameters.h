#ifndef RGS_PARAMETERS_H
#define RGS_PARAMETERS_H

#include "utilities/Types.h"
#include "core/Config.h"
#include "parameters/Gas.h"
#include "parameters/ImpulseSphere.h"

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
        auto gasesSize = Config::getInstance()->getGases().size();

        _pressure.resize(gasesSize);
        _density.resize(gasesSize);
        _temp.resize(gasesSize);
        _flow.resize(gasesSize);
        _heatFlow.resize(gasesSize);
    };

    void set(int gi, double pressure, double density, double temp) {
        _pressure[gi] = pressure;
        _density[gi] = density;
        _temp[gi] = temp;
    }

    void set(int gi, double pressure, double density, double temp, Vector3d flow, Vector3d heatFlow) {
        set(gi, pressure, density, temp);
        _flow[gi] = std::move(flow);
        _heatFlow[gi] = std::move(heatFlow);
    }

    void reset() {
        std::fill(_pressure.begin(), _pressure.end(), 0.0);
        std::fill(_density.begin(), _density.end(), 0.0);
        std::fill(_temp.begin(), _temp.end(), 0.0);
        std::fill(_flow.begin(), _flow.end(), Vector3d());
        std::fill(_heatFlow.begin(), _heatFlow.end(), Vector3d());
    }

    double getPressure(int gi) const {
        return _pressure[gi];
    }

    void setPressure(int gi, double pressure) {
        _pressure[gi] = pressure;
    }

    double getDensity(int gi) const {
        return _density[gi];
    }

    void setDensity(int gi, double density) {
        _density[gi] = density;
    }

    double getTemp(int gi) const {
        return _temp[gi];
    }

    void setTemp(int gi, double temp) {
        _temp[gi] = temp;
    }

    const Vector3d& getFlow(int gi) const {
        return _flow[gi];
    }

    void setFlow(int gi, const Vector3d& flow) {
        _flow[gi] = flow;
    }

    const Vector3d& getHeatFlow(int gi) const {
        return _heatFlow[gi];
    }

    void setHeatFlow(int gi, const Vector3d& heatFlow) {
        _heatFlow[gi] = heatFlow;
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
