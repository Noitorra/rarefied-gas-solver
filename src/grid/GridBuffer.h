#ifndef RGS_GRIDBUFFER_H
#define RGS_GRIDBUFFER_H

#include "CellResults.h"

#include <vector>

class GridBuffer {
private:
    CellResults _averageResults;
    std::vector<CellResults*> _allResults;

public:
    GridBuffer() = default;

    void clearAllResults() {
        _allResults.clear();
    }

    void addResults(CellResults* results) {
        _allResults.push_back(results);
    }

    const CellResults& getAverageResults() const {
        return _averageResults;
    }

    void calculateAverage();

private:
    void calculateAverageSingle();

};

#endif //RGS_GRIDBUFFER_H
