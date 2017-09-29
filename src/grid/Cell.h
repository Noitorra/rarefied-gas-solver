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

    std::vector<DoubleVector> _values;
    std::vector<DoubleVector> _halfValues;

    std::vector<ComputationType> _computationType;

    Config* _config;
    CellData* _data;

    CellParameters _resultParams;

public:
    explicit Cell(CellData* data);

    void init();

    void link(int axis, Cell* prevCell, Cell* nextCell);

    CellData* getData();

    CellParameters& getResultParams();

    char getComputationTypeAsCode(int axis) const;

    void computeType(int axis);

    void computeValue(int axis);

    void computeHalf(int axis);

    void computeIntegral(unsigned int gi0, unsigned int gi1);

    void computeBetaDecay(unsigned int gi0, unsigned int gi1, double lambda);

    bool checkValuesRange();

    std::vector<DoubleVector>& getValues();

    std::vector<DoubleVector>& getHalfValues();

private:
    void compute_type(int axis);

    void compute_half_left(int axis);

    void compute_half_normal(int axis);

    void compute_half_preright(int axis);

    void compute_half_right(int axis);

    void compute_half_diffuse_left(int axis, unsigned int gi);

    void compute_half_diffuse_right(int axis, unsigned int gi);

    void compute_half_gase_left(int axis, unsigned int gi);

    void compute_half_gase_right(int axis, unsigned int gi);

    void compute_half_flow_left(int axis, unsigned int gi);

    void compute_half_flow_right(int axis, unsigned int gi);

    void compute_half_mirror_left(int axis, unsigned int gi);

    void compute_half_mirror_right(int axis, unsigned int gi);

    void compute_value_normal(int axis);

    double compute_exp(const double& mass, const double& temp, const Vector3d& impulse);

    double limiter(const double& x, const double& y, const double& z);

    double compute_concentration(unsigned int gi);

    double compute_temperature(unsigned int gi, double density, const Vector3d& stream);

    double compute_pressure(unsigned int gi, double density, double temperature);

    Vector3d compute_stream(unsigned int gi);

    Vector3d compute_heatstream(unsigned int gi);

};

#endif /* CELL_H_ */
