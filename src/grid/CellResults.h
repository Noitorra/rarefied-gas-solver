#ifndef RGS_CELLRESULTS_H
#define RGS_CELLRESULTS_H

#include "CellParameters.h"

class CellResults : public CellParameters {
    friend class boost::serialization::access;

private:
    int _id;

public:
    CellResults() = default;

    explicit CellResults(int id) : CellParameters(), _id(id) {}

    int getId() const {
        return _id;
    }

    void setId(int id) {
        _id = id;
    }

    void setFromResults(const CellResults& results) {
        setId(results.getId());
        CellParameters::setFromParameters(results);
    }

private:
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & boost::serialization::base_object<CellParameters>(*this);
        ar & _id;
    }

};


#endif //RGS_CELLRESULTS_H
