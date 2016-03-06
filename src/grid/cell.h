#ifndef CELL_H_
#define CELL_H_

#include "main.h"
#include "utilities/types.h"

class Solver;
class GridManager;
class Grid;

typedef std::vector<double> DoubleVector;

class Cell {
  friend class GridManager;
  friend class Grid;
  friend class OutResults;
public:
	Cell();

	// initialize
	void Init(GridManager* pGridManager);
  void setParameters(double dPressure, double dTemperature, Vector3d dAreastep, int iGasIndex = 0, int iLockedAxes = -1);
  void setBoundaryType(sep::BoundaryType eBoundaryType, double dTemperature, Vector3d dStream, double dPressure, int iGasIndex = 0);

	void computeType(unsigned int dim);
	void computeValue(unsigned int dim);
	void computeHalf(unsigned int dim);
  void computeIntegral(unsigned int gi0, unsigned int gi1);
	void computeBetaDecay(unsigned int gi0, unsigned int gi1, double lambda);

	// checks
	bool checkInnerValuesRange();

	// macro data
	void computeMacroData();
	const std::vector<MacroData>& getMacroData() const { return m_vMacroData; }
private:
  enum CellType {
    CT_UNDEFINED,
    CT_LEFT,
    CT_NORMAL,
    CT_PRERIGHT,
    CT_RIGHT
  };

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

	void compute_half_flow_left(unsigned int dim, int gi);
	void compute_half_flow_right(unsigned int dim, int gi);

  void compute_half_mirror_left(unsigned int dim, int gi);
  void compute_half_mirror_right(unsigned int dim, int gi);

	void compute_value_normal(unsigned int dim);

	double compute_exp(const double& mass, const double& temp, const Vector3d& impulse);
  
  bool is_normal();

	inline double limiter(const double& x, const double& y, const double& z) {
		return limiter_superbee(x, y, z);
	}
	double limiter_superbee(const double& x, const double& y, const double& z);

	double compute_concentration(unsigned int gi);
	double compute_temperature(unsigned int gi);
  double compute_pressure(unsigned int gi);
	Vector3d compute_stream(unsigned int gi);
	Vector3d compute_heatstream(unsigned int gi);
  
  // Variables
  std::vector<double> m_vStartPressure;
  std::vector<double> m_vStartTemperature;
  Vector3d m_vAreastep;

  std::vector<Cell*> m_pPrev;
  std::vector<Cell*> m_pNext;

  std::vector<DoubleVector > m_vValue;
  std::vector<DoubleVector > m_vHalf;

  std::vector<CellType> m_vType;

  std::vector<sep::BoundaryType> m_vBoundaryType;
  std::vector<double> m_vBoundaryTemperature;
  std::vector<double> m_vBoundaryPressure;
  std::vector<Vector3d> m_vBoundaryStream;

  std::vector<MacroData> m_vMacroData;

  int m_iLockedAxes;

  GridManager* m_pGridManager;
  Solver* m_pSolver;
  Grid* m_pGrid;
};

#endif /* CELL_H_ */
