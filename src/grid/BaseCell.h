#ifndef RGS_BASECELL_H
#define RGS_BASECELL_H

#include "utilities/Types.h"

#include <vector>
#include <memory>

class CellConnection;

class BaseCell {
public:
    enum class Type {
        NORMAL,
        BORDER,
        PARALLEL
    };

protected:
    Type _type;
    int _id;
    std::vector<std::vector<double>> _values;
    std::vector<std::shared_ptr<CellConnection>> _connections;

public:
    BaseCell(Type type, int id);

    int getId() const;

    Type getType() const;

    std::vector<std::vector<double>>& getValues();

    const std::vector<std::shared_ptr<CellConnection>>& getConnections() const;

    void addConnection(CellConnection* connection);

    void check();

    virtual void init() = 0;
    virtual void computeTransfer() = 0;
    virtual void computeIntegral(unsigned int gi0, unsigned int gi1) = 0;
    virtual void computeBetaDecay(unsigned int gi0, unsigned int gi1, double lambda) = 0;

protected:
    double fast_exp(double mass, double temp, const Vector3d& impulse);
};


#endif //RGS_BASECELL_H
