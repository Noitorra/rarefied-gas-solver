#ifndef RGS_RESULTSPRINTER_H
#define RGS_RESULTSPRINTER_H

#include <string>
#include <vector>

class Mesh;
class CellResults;

class ResultsFormatter {
private:
    enum Param {
        PRESSURE,
        DENSITY,
        TEMPERATURE,
        FLOW,
        HEATFLOW
    };

    enum Type {
        DATA,
        PIC
    };

    std::string _root;
    std::string _main;
    std::vector<std::string> _params;
    std::vector<std::string> _types;
    std::string _gas;

public:
    ResultsFormatter();

    void writeAll(Mesh* mesh, const std::vector<CellResults>& results, unsigned int iteration);

};


#endif //RGS_RESULTSPRINTER_H
