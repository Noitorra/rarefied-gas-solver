#ifndef CELL_H_
#define CELL_H_

#include "types.h"
#include "main.h"

class VesselGrid;
class Solver;
class GridManager;
class Grid;

class Cell {
  friend class GridManager;
  friend class Grid;
  friend class VesselGrid;
  friend class LeftVesselGrid;
  friend class RightVesselGrid;
  friend class OutResults;
public:
	enum CellType {
		CT_UNDEFINED,
		CT_LEFT,
		CT_NORMAL,
		CT_PRERIGHT,
		CT_RIGHT
	};

	typedef std::vector< Cell* > CellVector;
	typedef std::vector<double> DoubleVector;
private:
	double m_dStartConcentration;
	double m_dStartTemperature;
  Vector3d m_vAreastep;

	// variables ... ?
	std::vector< CellVector > m_vNext;
	std::vector< CellVector > m_vPrev;

	std::vector< DoubleVector > m_vValue;
	std::vector< DoubleVector > m_vHalf;

	std::vector< CellType > m_vType;

  sep::BoundaryType m_eBoundaryType;
  double m_dBoundaryTemperature;
  double m_dBoundaryStream;
  double m_dBoundaryPressure;

	std::vector<MacroData> m_vMacroData;

public:
	Cell();
	virtual ~Cell();

	void setGridManager(GridManager* pGridManager) { m_pGridManager = pGridManager; }
	GridManager* getGridManager() { return m_pGridManager; }

	// main methods
	/* set all necessary parameters */
  void setParameters(double _Concentration, double _Temperature, Vector3d _Areastep);
  void setBoundaryType(sep::BoundaryType eBoundaryType, double dTemperature, double dStream, double dPressure);
  void setBoundaryType(sep::BoundaryType eBoundaryType, double dTemperature);
	/* creates cells inner values, takes long time */
	void Init(GridManager* pGridManager);

	void computeType(unsigned int dim);
	void computeValue(unsigned int dim);
	void computeHalf(unsigned int dim);
  void computeIntegral(unsigned int gi0, unsigned int gi1);

	// tests
	bool testInnerValuesRange();

	// macro data
	void computeMacroData();
	const std::vector<MacroData>& getMacroData() const { return m_vMacroData; }
private:
	// help methods
	void compute_type(unsigned int dim);

  void compute_half_left(unsigned int dim);
	void compute_half_normal(unsigned int dim);
	void compute_half_preright(unsigned int dim);
	void compute_half_right(unsigned int dim);

//	void computeValue_Left(unsigned int dim);
	void compute_value_normal(unsigned int dim);
//	void computeValue_PreRight(unsigned int dim);
//	void computeValue_Right(unsigned int dim);

	// help methods
  // help enum, not to fail this shit
  enum AverageDepth {
    AD_NEXT,
    AD_NEXTNEXT,
    AD_PREV,
    AD_PREVPREV
  };

  double compute_av(unsigned int dim, unsigned int gi, unsigned int ii, AverageDepth eAverageDepth);
  double compute_ah(unsigned int dim, unsigned int gi, unsigned int ii, AverageDepth eAverageDepth);

	double fast_exp(const double& mass, const double& temp, const Vector3d& impulse);
  
  // TODO: Change function name =)
  void ResetSpeed(unsigned int gi, double dConcentration, double dTemperature);

	template <typename T>
	inline int sgn(T val) {
	    return (T(0) < val) - (val < T(0));
	}
	inline double limiter(const double& x, const double& y, const double& z) {
		return limiter_superbee(x, y, z);
	}
	double limiter_superbee(const double& x, const double& y, const double& z);

	double compute_concentration(unsigned int gi);
	double compute_temperature(unsigned int gi);
	Vector3d compute_stream(unsigned int gi);
	Vector3d compute_heatstream(unsigned int gi);
  
  // Members should be last but not the least =)
  GridManager* m_pGridManager;
  Solver* m_pSolver;
  Grid* m_pGrid;
};

#endif /* CELL_H_ */
