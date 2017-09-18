#ifndef RGS_RESULTSPRINTER_H
#define RGS_RESULTSPRINTER_H

#include <string>

template <class T>
class Grid;

class CellParameters;

class ResultsWriter {
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
    ResultsWriter();

    void writeAll(Grid<CellParameters>* grid, unsigned int iteration);
};


#endif //RGS_RESULTSPRINTER_H
