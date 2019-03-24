#ifndef RGS_GRADIENTPARAMETER_H
#define RGS_GRADIENTPARAMETER_H

#include <string>
#include <vector>
#include <ostream>

#include <boost/serialization/access.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>

#include "utilities/Types.h"

class GradientParameter {
    friend class boost::serialization::access;

private:
    double _valueStart;
    double _valueEnd;
    Vector3d _pointStart;
    Vector3d _pointEnd;

public:
    GradientParameter() = default;

    GradientParameter(double valueStart, double valueEnd, Vector3d pointStart, Vector3d pointEnd) : _valueStart(valueStart),
                                                                                                    _valueEnd(valueEnd),
                                                                                                    _pointStart(std::move(pointStart)),
                                                                                                    _pointEnd(std::move(pointEnd)) {}

    double getValueStart() const {
        return _valueStart;
    }

    void setValueStart(double valueStart) {
        _valueStart = valueStart;
    }

    double getValueEnd() const {
        return _valueEnd;
    }

    void setValueEnd(double valueEnd) {
        _valueEnd = valueEnd;
    }

    const Vector3d& getPointStart() const {
        return _pointStart;
    }

    void setPointStart(const Vector3d& pointStart) {
        _pointStart = pointStart;
    }

    const Vector3d& getPointEnd() const {
        return _pointEnd;
    }

    void setPointEnd(const Vector3d& pointEnd) {
        _pointEnd = pointEnd;
    }

    double getValue(const Vector3d& point) const {
        double value = 0.0;

        Vector3d gradient = getPointEnd() - getPointStart();
        Vector3d gradientPoint = point - getPointStart();
        double proj = gradientPoint.scalar(gradient) / gradient.moduleSquare();
        if (proj <= 0.0) {
            value = getValueStart();
        } else if (proj >= 1.0) {
            value = getValueEnd();
        } else {
            value = getValueStart() + proj * (getValueEnd() - getValueStart());
        }

        return value;
    }

private:
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & _valueStart;
        ar & _valueEnd;
        ar & _pointStart;
        ar & _pointEnd;
    }
};


#endif //RGS_GRADIENTPARAMETER_H
