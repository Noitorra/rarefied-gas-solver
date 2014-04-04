/*
 * solver_info.h
 *
 *  Created on: 04 апр. 2014 г.
 *      Author: kisame
 */

#ifndef SOLVER_INFO_H_
#define SOLVER_INFO_H_


#include <vector>
#include <memory>

class Options;
class Impulse;
class Gas;


class SolverInfo {
private:
	std::shared_ptr<Impulse> m_pImpulse;
	std::shared_ptr<Options> m_pOptions;
	std::vector< std::shared_ptr<Gas> > m_vGas;
public:
	SolverInfo();
	virtual ~SolverInfo();

	Options* getOptions() { return m_pOptions.get(); }
	Impulse* getImpulse() { return m_pImpulse.get(); }
	std::vector< std::shared_ptr<Gas> >& getGasVector() { return m_vGas; }
};

#endif /* SOLVER_INFO_H_ */
