#ifndef RGS_IMPULSESPHERE_H
#define RGS_IMPULSESPHERE_H

#include "utilities/Types.h"

#include <ostream>

class ImpulseSphere {
    friend class boost::serialization::access;

private:
    double _maxImpulse;
    unsigned int _resolution;

    double _deltaImpulse;
    double _deltaImpulseQube;
    std::vector<Vector3d> _impulses;
    std::vector<Vector3i> _i2xyz;
    int*** _xyz2i;

public:
    ImpulseSphere() = default;

    ImpulseSphere(double maxImpulse, unsigned int resolution) : _maxImpulse(maxImpulse), _resolution(resolution) {}

    virtual ~ImpulseSphere() {
        if (_xyz2i != nullptr) {
            for (unsigned int x = 0; x < _resolution; x++) {
                for (unsigned int y = 0; y < _resolution; y++) {
                    delete[] _xyz2i[x][y];
                }
                delete[] _xyz2i[x];
            }
            delete[] _xyz2i;
        }
    }

    void init();

    double getMaxImpulse() const {
        return _maxImpulse;
    }

    unsigned int getResolution() const {
        return _resolution;
    }

    int*** getXYZ2I() const {
        return _xyz2i;
    }

    double getDeltaImpulse() const {
        return _deltaImpulse;
    }

    double getDeltaImpulseQube() const {
        return _deltaImpulseQube;
    }

    const std::vector<Vector3d>& getImpulses() const {
        return _impulses;
    }

    int reverseIndex(int ii, const Vector3d& normal);

    friend std::ostream& operator<<(std::ostream& os, const ImpulseSphere& impulse);

private:
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & _maxImpulse;
        ar & _resolution;
    }
};

#endif /* RGS_IMPULSESPHERE_H */
