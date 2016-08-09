#pragma once

#include "main.h"
#include "grid_constructor.h"

class Grid;

class Solver;

class Config;

class GridManager : public GridConstructor {
public:
    GridManager();

    void Init(Solver* pSolver);

    void ConfigureGrid();

    void PrintLinkage(sep::Axis axis);

    Grid* GetGrid() const;

    Solver* GetSolver() const;

private:
    void PushTemperature(double t);

    double PopTemperature();

    double GetTemperature();

    void PushPressure(double c);

    double PopPressure();

    double GetPressure();

    void AddBox(Vector2d p, Vector2d size, GridBox* box);

    //void SetBox(Vector2d p, Vector2d size, ConfigFunction config_func);
    void PrintGrid();

    void GridGeometryToInitialCells();

    void AdoptInitialCells();

    void FindNeighbour(Vector2i p, sep::CellType type, sep::Axis& axis, sep::NeighborType& neighbor, int& quant);

    bool FindNeighbourWithIndex(Vector2i p, sep::CellType type, int index, sep::Axis& axis, sep::NeighborType& neighbor);

    bool GetNeighbour(Vector2i p, sep::Axis axis, sep::NeighborType neighbor, sep::CellType& type);

    int GetSlash(sep::NeighborType type) const;

    void FillInGrid();

    void LinkCells();

    void InitCells();

    void LinkNeighbors(Vector2i p, sep::Axis axis, sep::NeighborType eNeighbor);

    void LinkToMyself(Vector2i p, sep::Axis mirror_axis);

    void PrintInfo();

    std::vector<double> m_vTemperatures;
    std::vector<double> m_vPressures;
    std::vector<GridBox*> m_vBoxes;

    Grid* m_pGrid;
    Solver* m_pSolver;
    Config* m_pConfig;
};
