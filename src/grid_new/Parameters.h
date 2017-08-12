#ifndef RGS_PARAMETERS_H
#define RGS_PARAMETERS_H

#include <utilities/types.h>

class Parameters {
    friend class boost::serialization::access;

private:
    double _pressure;
    double _density;
    double _temp;
    Vector3d _flow;
    Vector3d _heatFlow;

public:
    Parameters() {
        _heatFlow = Vector3d();
    }

    Parameters(double pressure, double density, double temp) : Parameters() {
        _pressure = pressure;
        _density = density;
        _temp = temp;
    }

    Parameters(double pressure, double density, double temp, Vector3d flow) : Parameters(pressure, density, temp) {
        _flow = flow;
    }

    double getPressure() const {
        return _pressure;
    }

    void setPressure(double pressure) {
        _pressure = pressure;
    }

    double getDensity() const {
        return _density;
    }

    void setDensity(double density) {
        _density = density;
    }

    double getTemp() const {
        return _temp;
    }

    void setTemp(double temp) {
        _temp = temp;
    }

    const Vector3d& getFlow() const {
        return _flow;
    }

    void setFlow(const Vector3d& flow) {
        _flow = flow;
    }

    const Vector3d& getHeatFlow() const {
        return _heatFlow;
    }

    void setHeatFlow(const Vector3d& heatFlow) {
        _heatFlow = heatFlow;
    }

private:
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & _pressure;
        ar & _density;
        ar & _temp;
        ar & _flow;
    }

};

#endif //RGS_PARAMETERS_H
