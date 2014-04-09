/*
 * cell.h
 *
 *  Created on: 04 апр. 2014 г.
 *      Author: kisame
 */

#ifndef CELL_H_
#define CELL_H_

#include "types.h"
#include "main.h"

class Grid;
class GridManager;
class VesselGrid;
class LeftVesselGrid;
class RightVesselGrid;

class Cell {
  friend GridManager;
  friend Grid;
  friend VesselGrid;
  friend LeftVesselGrid;
  friend RightVesselGrid;
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

	std::vector<MacroData> m_vMacroData;

	GridManager* m_pGridManager;
public:
	Cell(GridManager* _GridManager);
	virtual ~Cell();

	void setGridManager(GridManager* _GridManager) { m_pGridManager = _GridManager; }
	GridManager* getGridManager() { return m_pGridManager; }
  Grid* getGrid() const;

	// main methods
	/* set all necessary parameters */
  void setParameters(double _Concentration, double _Temperature, Vector3d _Areastep);
	/* creates cells inner values, takes long time */
	void Init();

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
  Cell();
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
	double compute_apv(CellVector& cellv, unsigned int dim, unsigned int gi, unsigned int ii, unsigned int depth = 0 );
	double compute_anv(CellVector& cellv, unsigned int dim, unsigned int gi, unsigned int ii, unsigned int depth = 0 );
	double compute_aph(CellVector& cellv, unsigned int dim, unsigned int gi, unsigned int ii, unsigned int depth = 0 );
	double compute_anh(CellVector& cellv, unsigned int dim, unsigned int gi, unsigned int ii, unsigned int depth = 0 );

	double fast_exp(const double& mass, const double& temp, const Vector3d& impulse);

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
};

#endif /* CELL_H_ */
