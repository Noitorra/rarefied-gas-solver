#ifndef IMPULSE_H_
#define IMPULSE_H_

#include "types.h"

class GridManager;
class Grid;
class Solver;
class Config;

typedef std::vector<Vector3d> ImpulseVector;

class Impulse {
public:
	Impulse();
	virtual ~Impulse();

	void Init(GridManager* pGridManager);

	void setMaxImpulse(double max_impulse);
	void setResolution(unsigned int resolution);

	unsigned int getResolution();
	double getMaxImpulse();
  int*** getXYZ2I() { return m_pxyz2i; }

	double getDeltaImpulse();
	double getDeltaImpulseQube();
	ImpulseVector& getVector();
private:
	// setting & getting
	double m_dMaxImpulse;
	unsigned int m_uResolution;
  
	// getting
	double m_dDeltaImpulse;
	double m_dDeltaImpulseQube;
	ImpulseVector m_vImpulse;
  int*** m_pxyz2i;
  
  GridManager* m_pGridManager;
  Grid* m_pGrid;
  Solver* m_pSolver;
  Config* m_pConfig;
};

#endif /* IMPULSE_H_ */
