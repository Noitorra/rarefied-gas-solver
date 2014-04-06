/*
 * impulse.h
 *
 *  Created on: 04 апр. 2014 г.
 *      Author: kisame
 */

#ifndef IMPULSE_H_
#define IMPULSE_H_

#include "types.h"

class SolverInfo;

typedef std::vector< Vector3d > ImpulseVector;

class Impulse {
private:
	// setting & getting
	double m_dMaxImpulse;
	unsigned int m_uResolution;

	// getting
	double m_dDeltaImpulse;
	double m_dDeltaImpulseQube;
	ImpulseVector m_vImpulse;

	SolverInfo* m_cSolverInfo;
public:
	Impulse();
	virtual ~Impulse();

	void Init();
	void setSolverInfo(SolverInfo* argSolverInfo);

	void setMaxImpulse(double max_impulse);
	void setResolution(unsigned int resolution);

	unsigned int getResolution();
	double getMaxImpulse();

	double getDeltaImpulse();
	double getDeltaImpulseQube();
	ImpulseVector& getVector();
};

#endif /* IMPULSE_H_ */
