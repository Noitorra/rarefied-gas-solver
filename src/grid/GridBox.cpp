#include "GridBox.h"

GridBox::GridBox(const Vector2d& point, const Vector2d& size, bool isSolid)
        : _point(point), _size(size), _isSolid(isSolid) {}

const Vector2d &GridBox::getPoint() const {
    return _point;
}

const Vector2d &GridBox::getSize() const {
    return _size;
}

bool GridBox::isSolid() const {
    return _isSolid;
}

void GridBox::setMainFunction(const GridBox::SetupFunction& mainFunction) {
    _mainFunction = mainFunction;
}

void GridBox::setLeftBorderFunction(const GridBox::BorderSetupFunction& leftBorderFunction) {
    _leftBorderFunction = leftBorderFunction;
}

void GridBox::setRightBorderFunction(const GridBox::BorderSetupFunction& rightBorderFunction) {
    _rightBorderFunction = rightBorderFunction;
}

void GridBox::setTopBorderFunction(const GridBox::BorderSetupFunction& topBorderFunction) {
    _topBorderFunction = topBorderFunction;
}

void GridBox::setBottomBorderFunction(const GridBox::BorderSetupFunction& bottomBorderFunction) {
    _bottomBorderFunction = bottomBorderFunction;
}

const GridBox::SetupFunction& GridBox::getMainFunction() const {
    return _mainFunction;
}

const GridBox::BorderSetupFunction& GridBox::getLeftBorderFunction() const {
    return _leftBorderFunction;
}

const GridBox::BorderSetupFunction& GridBox::getRightBorderFunction() const {
    return _rightBorderFunction;
}

const GridBox::BorderSetupFunction& GridBox::getTopBorderFunction() const {
    return _topBorderFunction;
}

const GridBox::BorderSetupFunction& GridBox::getBottomBorderFunction() const {
    return _bottomBorderFunction;
}

void GridBox::setPoint(const Vector2d& point) {
    _point = point;
}

void GridBox::setSize(const Vector2d& size) {
    _size = size;
}

void GridBox::setSolid(bool isSolid) {
    _isSolid = isSolid;
}

