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

class Impulse {
private:
	// setting & getting
	double m_dMaxImpulse;
	unsigned int m_uResolution;

	// getting
	double m_dDeltaImpulse;
	double m_dDeltaImpulseQube;
	std::vector< Vector3d > m_vImpulse;

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
	std::vector< Vector3d >& getVector();
};

#endif /* IMPULSE_H_ */
