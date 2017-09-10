#ifndef RGS_GRIDMAKER_H
#define RGS_GRIDMAKER_H

#include <utilities/types.h>
#include <functional>
#include "Grid.h"

class CellData;

class GridMaker {
private:
    typedef std::function<void(Vector2d point, CellData& data)> SetupFunction;
    typedef std::function<void(double point, CellData& data)> BorderSetupFunction;

    class ConfigBox {
    private:
        Vector2d _point;
        Vector2d _size;
        bool _isSolid;
        SetupFunction _mainFunction;
        BorderSetupFunction _leftBorderFunction;
        BorderSetupFunction _rightBorderFunction;
        BorderSetupFunction _topBorderFunction;
        BorderSetupFunction _bottomBorderFunction;
    public:
        ConfigBox(const Vector2d& point, const Vector2d& size, bool isSolid);

        void setMainFunction(const SetupFunction& mainFunction);

        void setLeftBorderFunction(const BorderSetupFunction& leftBorderFunction);

        void setRightBorderFunction(const BorderSetupFunction& rightBorderFunction);

        void setTopBorderFunction(const BorderSetupFunction& topBorderFunction);

        void setBottomBorderFunction(const BorderSetupFunction& bottomBorderFunction);

        const SetupFunction& getMainFunction() const;

        const BorderSetupFunction& getLeftBorderFunction() const;

        const BorderSetupFunction& getRightBorderFunction() const;

        const BorderSetupFunction& getTopBorderFunction() const;

        const BorderSetupFunction& getBottomBorderFunction() const;

        const Vector2d& getPoint() const;

        const Vector2d& getSize() const;

        bool isSolid() const;

        void setPoint(const Vector2d& point);

        void setSize(const Vector2d& size);

        void setIsSolid(bool isSolid);
    };

public:
    static Grid<CellData>* makeGrid(const Vector2u& size, unsigned int gasesCount);

private:
    static Grid<CellData>* makeWholeGrid(const Vector2u& size, unsigned int gasesCount);

    static std::vector<ConfigBox*> makeBoxes();

    static std::vector<Grid<CellData>*> splitGrid(Grid<CellData>* grid, unsigned int numGrids);
};


#endif //RGS_GRIDMAKER_H
