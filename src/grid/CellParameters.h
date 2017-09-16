#ifndef RGS_PARAMETERS_H
#define RGS_PARAMETERS_H

#include <utilities/types.h>

#include <utility>

class CellParameters {
    friend class boost::serialization::access;

private:
    double _pressure = 0.0;
    double _density = 0.0;
    double _temp = 0.0;
    Vector3d _flow;
    Vector3d _heatFlow;

public:
    CellParameters() = default;

    void set(double pressure, double density, double temp) {
        _pressure = pressure;
        _density = density;
        _temp = temp;
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
        ar & _heatFlow;
    }

};

#endif //RGS_PARAMETERS_H
