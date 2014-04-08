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

class Solver;

class Options;
class Impulse;
class Gas;
class Config;

typedef std::vector< std::shared_ptr<Gas> > GasVector;

class SolverInfo {
private:
	std::shared_ptr<Impulse> m_pImpulse;
	std::shared_ptr<Options> m_pOptions;
	GasVector m_vGas;

	Solver* m_pSolver;
public:
	SolverInfo();
	virtual ~SolverInfo();

	void Init();

	Options* getOptions() { return m_pOptions.get(); }
	Impulse* getImpulse() { return m_pImpulse.get(); }
	GasVector& getGasVector() { return m_vGas; }
  Config* GetConfig() const;

	Solver* getSolver() {
		return m_pSolver;
	}

	void setSolver(Solver* pSolver) {
		m_pSolver = pSolver;
	}
};

#endif /* SOLVER_INFO_H_ */
