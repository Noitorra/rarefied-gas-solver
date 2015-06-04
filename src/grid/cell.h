#ifndef CELL_H_
#define CELL_H_

#include "main.h"
#include "utilities/types.h"

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
  std::vector<double> m_vStartPressure;
  std::vector<double> m_vStartTemperature;
  Vector3d m_vAreastep;

	// variables ... ?
	std::vector< CellVector > m_vNext;
	std::vector< CellVector > m_vPrev;

	std::vector< DoubleVector > m_vValue;
	std::vector< DoubleVector > m_vHalf;

	std::vector< CellType > m_vType;

  std::vector<sep::BoundaryType> m_vBoundaryType;
  std::vector<double> m_vBoundaryTemperature;
  std::vector<double> m_vBoundaryPressure;
	std::vector<Vector3d> m_vBoundaryStream;
	std::vector<MacroData> m_vMacroData;
	int m_iLockedAxes;	// will not make compute half/value for that axes

public:
	Cell();
	virtual ~Cell();

	void setGridManager(GridManager* pGridManager) { m_pGridManager = pGridManager; }
	GridManager* getGridManager() { return m_pGridManager; }

	// main methods
	/* set all necessary parameters */
  void setParameters(double _Pressure, double _Temperature, Vector3d _Areastep, int _GasIndex = 0, int locked_axes = -1);
  void setBoundaryType(sep::BoundaryType eBoundaryType, double dTemperature, Vector3d dStream, double dPressure, int iGasIndex = 0);
	/* creates cells inner values, takes long time */
	void Init(GridManager* pGridManager);

	void computeType(unsigned int dim);
	void computeValue(unsigned int dim);
	void computeHalf(unsigned int dim);
  void computeIntegral(unsigned int gi0, unsigned int gi1);
	void computeBetaDecay(unsigned int gi0, unsigned int gi1, double lambda);

	// tests
	bool testInnerValuesRange();

	// macro data
	void computeMacroData();
  bool isValid();
	const std::vector<MacroData>& getMacroData() const { return m_vMacroData; }
private:
	// help methods
  void compute_type(unsigned int dim);

  void compute_half_left(unsigned int dim);
	void compute_half_normal(unsigned int dim);
	void compute_half_preright(unsigned int dim);
	void compute_half_right(unsigned int dim);

  void compute_half_diffuse_left(unsigned int dim, int gi);
  void compute_half_diffuse_right(unsigned int dim, int gi);

	void compute_half_gase_left(unsigned int dim, int gi);
	void compute_half_gase_right(unsigned int dim, int gi);

  void compute_half_mirror_left(unsigned int dim, int gi);
  void compute_half_mirror_right(unsigned int dim, int gi);

	void compute_half_vacuum_left(unsigned int dim, int gi);
	void compute_half_vacuum_right(unsigned int dim, int gi);

	void compute_value_normal(unsigned int dim);

	// help methods
  // help enum, not to fail this shit
  enum AverageDepth {
    AD_NEXT,
    AD_NEXTNEXT,
    AD_PREV,
    AD_PREVPREV
  };

  inline void cycleValue(double& result, unsigned int& count, const unsigned int& gi,
    const unsigned int& ii, const Cell::CellVector& cellVector);
  inline void cycleHalf(double& result, unsigned int& count, const unsigned int& gi,
    const unsigned int& ii, const Cell::CellVector& cellVector);

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
  double compute_pressure(unsigned int gi);
	Vector3d compute_stream(unsigned int gi);
	Vector3d compute_heatstream(unsigned int gi);
  
  // Members should be last but not the least =)
  GridManager* m_pGridManager;
  Solver* m_pSolver;
  Grid* m_pGrid;
};

#endif /* CELL_H_ */
