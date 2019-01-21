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
    BaseCell(Type type, int id) : _type(type), _id(id) {}

    int getId() const {
        return _id;
    }

    Type getType() const {
        return _type;
    }

    std::vector<std::vector<double>>& getValues() {
        return _values;
    }


    const std::vector<std::shared_ptr<CellConnection>>& getConnections() const {
        return _connections;
    }

    void addConnection(CellConnection* connection);

    void check();

    virtual void init() = 0;
    virtual void computeTransfer() = 0;
    virtual void computeIntegral(int gi0, int gi1) = 0;
    virtual void computeBetaDecay(int gi0, int gi1, double lambda) = 0;

};


#endif //RGS_BASECELL_H
