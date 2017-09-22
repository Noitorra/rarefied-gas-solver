#ifndef CELL_H_
#define CELL_H_

#include <ostream>
#include "utilities/Types.h"
#include "CellParameters.h"

class Config;
class CellData;

typedef std::vector<double> DoubleVector;

class Cell {
public:
    friend class OutResults;

    enum class ComputationType {
        UNDEFINED,
        LEFT,
        NORMAL,
        PRE_RIGHT,
        RIGHT
    };

private:
    std::vector<Cell*> _prev;
    std::vector<Cell*> _next;

    std::vector<DoubleVector> _value;
    std::vector<DoubleVector> _halfValue;

    std::vector<ComputationType> _computationType;

    Config* _config;
    CellData* _data;

    CellParameters _resultParams;

public:
    explicit Cell(CellData* data);

    void init();

    void link(unsigned int dim, Cell* prevCell, Cell* nextCell);

    CellData* getData();

    CellParameters& getResultParams();

    ComputationType getComputationType(unsigned int dim) const;

    void computeType(unsigned int dim);

    void computeValue(unsigned int dim);

    void computeHalf(unsigned int dim);

    void computeIntegral(unsigned int gi0, unsigned int gi1);

    void computeBetaDecay(unsigned int gi0, unsigned int gi1, double lambda);

    // checks
    bool checkInnerValuesRange();

private:
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
};

#endif /* CELL_H_ */
