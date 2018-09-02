#ifndef CELL_H_
#define CELL_H_

#include <ostream>
#include "utilities/Types.h"
#include "CellParameters.h"
#include "CellResults.h"

class Config;
class CellConnection;

typedef std::vector<double> DoubleVector;

class Cell {
public:
    enum class Type {
        NORMAL,
        BORDER,
        PARALLEL
    };
    enum class BoundaryType {
        DIFFUSE,
        PRESSURE,
        FLOW,
        MIRROR
    };

private:
    int _id;
    Type _type;
    CellParameters _params;
    CellParameters _boundaryParams;
    double _volume;

    std::vector<std::shared_ptr<CellConnection>> _connections;

    std::vector<DoubleVector> _values;
    std::vector<DoubleVector> _newValues;

    std::shared_ptr<CellResults> _results;

public:
    Cell(int id, Type type, double volume);

    void init();

    std::vector<DoubleVector>& getValues();

    int getId() const;

    Type getType() const;

    void addConnection(CellConnection* connection);

    void computeTransfer();

    void swapValues();

    void computeIntegral(unsigned int gi0, unsigned int gi1);

    void computeBetaDecay(unsigned int gi0, unsigned int gi1, double lambda);

    CellResults* getResults();

private:
    double fast_exp(const double& mass, const double& temp, const Vector3d& impulse);

    void compute_transfer_normal();

    void compute_transfer_border();

    double compute_density(unsigned int gi);

    double compute_temperature(unsigned int gi, double density, const Vector3d& stream);

    double compute_pressure(unsigned int gi, double density, double temperature);

    Vector3d compute_stream(unsigned int gi);

    Vector3d compute_heatstream(unsigned int gi);

};

#endif /* CELL_H_ */
