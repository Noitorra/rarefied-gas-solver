#ifndef RGS_RESULTSPRINTER_H
#define RGS_RESULTSPRINTER_H

#include <string>
#include <vector>

class Mesh;
class CellResults;

class ResultsFormatter {
private:
    enum class Param {
        PRESSURE,
        DENSITY,
        TEMPERATURE,
        FLOW,
        HEATFLOW
    };

    std::string _root;
    std::string _main;

    std::vector<Param> _scalarParams;
    std::vector<Param> _vectorParams;

    std::vector<CellResults*> _lastResults;

public:
    ResultsFormatter();

    void writeAll(unsigned int iteration, Mesh* mesh, const std::vector<CellResults*>& results);
    void writeMeshDetails(Mesh* mesh);
    void writeProgression(unsigned int iteration, const std::vector<CellResults*>& results);

};


#endif //RGS_RESULTSPRINTER_H
