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

    ImpulseSphere(double maxImpulse, unsigned int resolution);

    virtual ~ImpulseSphere();

    void init();

    double getMaxImpulse() const;

    unsigned int getResolution() const;

    double getDeltaImpulse() const;

    double getDeltaImpulseQube() const;

    const std::vector<Vector3d>& getImpulses() const;

    int*** getXYZ2I() const;

    int reverseIndex(int ii, Vector3d normal);

    friend std::ostream& operator<<(std::ostream& os, const ImpulseSphere& impulse);

private:
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & _maxImpulse;
        ar & _resolution;
    }
};

#endif /* RGS_IMPULSESPHERE_H */
