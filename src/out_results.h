#pragma once

#include "main.h"

class Grid;

class GridManager;

class Config;

class OutResults {
public:
    OutResults();

    void Init(Grid* pGrid, GridManager* pGridManager);

    void OutAll(int iIteration);

    void OutAverageStream(int iteration);

private:
    void LoadParameters();

    void OutParameter(sep::MacroParamType type, int gas, int index);

    double ComputeAverageColumnStream(int index_x, unsigned int gi, int start_y, int size_y);

    void OutAverageStreamComb(std::fstream& filestream, int gas_n);

    void OutAverageStreamGPRT(std::fstream& filestream, int gas_n);

    Grid* m_pGrid;
    GridManager* m_pGridManager;
    Config* m_pConfig;
};
