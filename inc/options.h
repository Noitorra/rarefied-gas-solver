/*
 * options.h
 *
 *  Created on: 04 апр. 2014 г.
 *      Author: kisame
 */

#ifndef OPTIONS_H_
#define OPTIONS_H_

class SolverInfo;

class Options {
	SolverInfo* m_cSolverInfo;
public:
	Options();
	virtual ~Options();

	void setSolverInfo(SolverInfo* _SolverInfo);
	SolverInfo* getSolverInfo();
};

#endif /* OPTIONS_H_ */
