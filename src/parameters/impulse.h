#ifndef IMPULSE_H_
#define IMPULSE_H_

#include "main.h"

class GridManager;

class Grid;

class Solver;

typedef std::vector<Vector3d> ImpulseVector;

class Impulse {
public:
    Impulse();

    virtual ~Impulse();

    void Init();

    void setMaxImpulse(double max_impulse);

    void setResolution(unsigned int resolution);

    unsigned int getResolution();

    double getMaxImpulse();

    int*** getXYZ2I() const {
        return m_pxyz2i;
    }

    double getDeltaImpulse();

    double getDeltaImpulseQube();

    ImpulseVector& getVector();

    int reverseIndex(int ii, sep::Axis eAxis);

private:
    // setting & getting
    double m_dMaxImpulse;
    unsigned int m_uResolution;

    // getting
    double m_dDeltaImpulse;
    double m_dDeltaImpulseQube;
    ImpulseVector m_vImpulse;
    int*** m_pxyz2i;
    std::vector<Vector3i> m_pi2xyz;
};

#endif /* IMPULSE_H_ */
