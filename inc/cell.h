/*
 * cell.h
 *
 *  Created on: 04 апр. 2014 г.
 *      Author: kisame
 */

#ifndef CELL_H_
#define CELL_H_

#include <types.h>

class Grid;

class Cell {
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
	double m_dStartDensity;
	double m_dStartTemperature;
	std::vector<double> m_vAreastep;
public:
	Cell();
	Cell(Grid* _Grid);
	virtual ~Cell();

	void setGrid(Grid* _Grid) { m_pGrid = _Grid; }
	Grid* getGrid() { return m_pGrid; }

	// variables ... ?
	std::vector< CellVector > m_vNext;
	std::vector< CellVector > m_vPrev;

	std::vector< DoubleVector > m_vValue;
	std::vector< DoubleVector > m_vHalf;

	// main methods
	/* set all necessary parameters */
	void setParameters(const double& _Density, const double& _Temperature, const DoubleVector& _Areastep);
	/* creates cells inner values, takes long time */
	void Init();

	void computeValue(unsigned int dim);
	void computeHalf(unsigned int dim);
	void computeIntegral(unsigned int dim);
private:
	// help methods
	void computeType(unsigned int dim);

	void computeHalf_Left(unsigned int dim);
	void computeHalf_Normal(unsigned int dim);
	void computeHalf_PreRight(unsigned int dim);
	void computeHalf_Right(unsigned int dim);

//	void computeValue_Left(unsigned int dim);
	void computeValue_Normal(unsigned int dim);
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
private:
	std::vector< CellType > m_vType;
	Grid* m_pGrid;

};

#endif /* CELL_H_ */
