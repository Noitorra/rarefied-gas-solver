#pragma once

#include "main.h"

class Grid;

class GridManager;

class Config;

class OutResults {
public:
    OutResults();

    void Init(Grid* grid, GridManager* grid_manager);

    void OutAll(int iteration);

    void OutAverageStream(int iteration);

private:
    void LoadParameters();

    void OutParameter(sep::MacroParamType type, int gas, int index);

    double ComputeAverageColumnStream(int index_x, unsigned int gi, int start_y, int size_y);

    void OutAverageStreamComb(std::fstream& filestream, int gas_n);

    void OutAverageStreamGPRT(std::fstream& filestream, int gas_n);

    Grid* grid_;
    GridManager* grid_manager_;
    Config* m_pConfig;
};
