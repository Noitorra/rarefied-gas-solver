#ifndef RGS_GRIDBOX_H
#define RGS_GRIDBOX_H

#include <utilities/types.h>
#include <functional>

class CellData;

class GridBox {
public:
    typedef std::function<void(Vector2d point, CellData& data)> SetupFunction;
    typedef std::function<void(double point, CellData& data)> BorderSetupFunction;

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
    GridBox(const Vector2d& point, const Vector2d& size, bool isSolid);

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

    void setSolid(bool isSolid);
};


#endif //RGS_GRIDBOX_H
