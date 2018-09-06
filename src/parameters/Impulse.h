#ifndef IMPULSE_H
#define IMPULSE_H

#include <ostream>
#include "utilities/Types.h"

class Impulse {
private:
    // setting & getting
    double m_dMaxImpulse;
    unsigned int m_uResolution;

    // getting
    double m_dDeltaImpulse;
    double m_dDeltaImpulseQube;
    std::vector<Vector3d> m_vImpulses;
    int*** m_pxyz2i;
    std::vector<Vector3i> m_pi2xyz;

public:
    Impulse();

    virtual ~Impulse();

    void Init();

    void setMaxImpulse(double dMaxImpulse);

    double getMaxImpulse() const;

    void setResolution(unsigned int uResolution);

    unsigned int getResolution() const;

    int*** getXYZ2I() const;

    double getDeltaImpulse() const;

    double getDeltaImpulseQube() const;

    const std::vector<Vector3d>& getVector() const;

//    int reverseIndex(int ii, unsigned int axis);

    friend std::ostream& operator<<(std::ostream& os, const Impulse& impulse);
};

#endif /* IMPULSE_H */
