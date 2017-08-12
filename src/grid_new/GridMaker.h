#ifndef RGS_GRIDMAKER_H
#define RGS_GRIDMAKER_H

#include <utilities/types.h>
#include <functional>

class Grid;

class CellWrapper;

class GridMaker {
private:
    typedef std::function<void(Vector2u point, Vector2u size, CellWrapper& config)> SetupFunction;

    class ConfigBox {
    private:
        Vector2d _point;
        Vector2d _size;
        SetupFunction _setupFunction;
    public:
        ConfigBox(const Vector2d& point, const Vector2d& size, SetupFunction setupFunction);

        const Vector2d& getPoint() const;

        const Vector2d& getSize() const;

        const SetupFunction& getSetupFunction() const;
    };

    typedef std::vector<ConfigBox*> ConfigBoxVector;

public:
    static Grid* makeGrid(const Vector2u& size, unsigned int gasesCount);

private:
    static Grid* makeWholeGrid(const Vector2u& size, unsigned int gasesCount);

    static ConfigBoxVector makeBoxes();

    static std::vector<Grid*> splitGrid(Grid* grid, unsigned int count);
};


#endif //RGS_GRIDMAKER_H
