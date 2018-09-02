#ifndef RGS_CELLRESULTS_H
#define RGS_CELLRESULTS_H

#include "CellParameters.h"

class CellResults : public CellParameters {
private:
    int _id;

public:
    explicit CellResults(int id) : CellParameters(), _id(id) {}

    int getId() const {
        return _id;
    }

    void setId(int id) {
        _id = id;
    }

};


#endif //RGS_CELLRESULTS_H
