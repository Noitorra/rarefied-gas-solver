#ifndef CELL_H_
#define CELL_H_

#include "utilities/types.h"

class Config;
class Parameters;
class CellWrapper;

typedef std::vector<double> DoubleVector;

class Cell {
    friend class OutResults;

public:
    enum BoundaryType {
        BT_DIFFUSE,
        BT_GASE,
        BT_MIRROR,
        BT_FLOW
    };

    Cell(CellWrapper* wrapper);

    // initialize
    void init(std::vector<Parameters> parameters, Vector3d dAreastep, int iLockedAxes);

    void setBoundaryType(BoundaryType eBoundaryType, const Parameters& params, int iGasIndex = 0);

    void computeType(unsigned int dim);

    void computeValue(unsigned int dim);

    void computeHalf(unsigned int dim);

    void computeIntegral(unsigned int gi0, unsigned int gi1);

    void computeBetaDecay(unsigned int gi0, unsigned int gi1, double lambda);

    // checks
    bool checkInnerValuesRange();

    // macro data
    std::vector<Parameters> computeMacroData();

private:
    enum class Type {
        UNDEFINED,
        LEFT,
        NORMAL,
        PRERIGHT,
        RIGHT
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

    double compute_temperature(unsigned int gi, double density, const Vector3d& stream);

    double compute_pressure(unsigned int gi, double density, double temperature);

    Vector3d compute_stream(unsigned int gi);

    Vector3d compute_heatstream(unsigned int gi);

    // Variables
    Vector3d m_vStep;
    int m_iLockedAxes;

    std::vector<Cell*> m_pPrev;
    std::vector<Cell*> m_pNext;

    std::vector<DoubleVector> m_vValue;
    std::vector<DoubleVector> m_vHalf;

    std::vector<Type> m_vType;

    Config* m_pConfig;

    CellWrapper* _wrapper;
};

#endif /* CELL_H_ */
