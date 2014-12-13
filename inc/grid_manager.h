#pragma once

#include "main.h"
#include "grid_constructor.h"

class Grid;
class Solver;

class GridManager : public GridConstructor {
public:
    GridManager();
    void Init();
    void ConfigureGrid();
    void PrintLinkage(sep::Axis axis);

    Grid* GetGrid() const { return grid_.get(); }
    Solver* GetSolver() const { return solver_.get(); }
  
private:
    void PushTemperature(double t);
    double PopTemperature();
    double GetTemperature();
    void PushConcentration(double c);
    double PopConcentration();
    double GetConcentration();
    void SetLooping();
    void SetBox(Vector2i p, Vector2i size,
            std::function<void(int x, int y, CellConfig* config)> config_func);
    void SetVessel();
    void PrintGrid();
    void GridGeometryToInitialCells();
    void AdoptInitialCells();
    void FindNeighbour(Vector2i p, sep::CellType type,
                     sep::Axis& axis,
                     sep::NeighborType& neighbor,
                     int& quant);
    bool FindNeighbourWithIndex(Vector2i p,
                              sep::CellType type,
                              int index,
                              sep::Axis& axis,
                              sep::NeighborType& neighbor);
    bool GetNeighbour(Vector2i p, sep::Axis axis,
                    sep::NeighborType neighbor,
                    sep::CellType& type);
    int GetSlash(sep::NeighborType type) const;
    void FillInGrid();
    void LinkCells();
    void InitCells();
    void LinkNeighbors(Vector2i p, sep::Axis axis,
          sep::NeighborType eNeighbor);

    std::vector<double> temperature_stack_;
    std::vector<double> concentration_stack_;
    std::shared_ptr<Grid> grid_;
    std::shared_ptr<Solver> solver_;

    struct GridBox {
        Vector2i p;
        Vector2i size;
        CellConfig def_config;

        std::function<void(int x, int y, CellConfig* config)> config_func;
    };
    std::vector<GridBox> boxes_stack_;
};