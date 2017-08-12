#ifndef RGS_GRID_H
#define RGS_GRID_H

#include "utilities/types.h"
#include <vector>
#include <ostream>

class CellWrapper;

class Grid {
private:
    std::vector<CellWrapper*> _data;
    Vector2u _size;
public:
    Grid() {}

    Grid(Vector2u size) : _size(size) {
        _data.resize(size.x() * size.y(), nullptr);
    }

    Grid(const std::vector<CellWrapper*>& data, Vector2u size) : _data(data), _size(size) {
        _data.resize(size.x() * size.y(), nullptr);
    }

    void resize(Vector2u point, Vector2u size) {
        std::vector<CellWrapper*> newData(size.x() * size.y(), nullptr);
        for (unsigned int x = 0; x < size.x(); x++) {
            for (unsigned int y = 0; y < size.y(); y++) {
                newData[toIndex({x, y}, size)] = get(x + point.x(), y + point.y());
            }
        }
        _data.clear();
        _data.insert(std::begin(_data), std::begin(newData), std::end(newData));
        _size = size;
    }

    CellWrapper* get(unsigned int x, unsigned int y) const {
        return get(Vector2u(x, y));
    }

    CellWrapper* get(const Vector2u& point) const {
        return _data[toIndex(point, _size)];
    }

    void set(unsigned int x, unsigned int y, CellWrapper* value) {
        set(Vector2u(x, y), value);
    }

    void set(const Vector2u& point, CellWrapper* value) {
        _data[toIndex(point, _size)] = value;
    }

    const Vector2u& getSize() const {
        return _size;
    }

    void setByIndex(CellWrapper* value, unsigned int index) {
        _data[index] = value;
    }

    CellWrapper* getByIndex(unsigned int index) const {
        return _data[index];
    }

    unsigned int getCount() const {
        return (unsigned int) _data.size();
    }

    unsigned int getRealCount() const {
        unsigned int realCount = 0;
        for (std::size_t i = 0; i < _data.size(); i++) {
            if (_data[i] != nullptr) {
                realCount++;
            }
        }
        return realCount;
    }

    friend std::ostream& operator<<(std::ostream& os, const Grid& grid);

    static inline unsigned int toIndex(const Vector2u& point, const Vector2u& size) {
        return point.x() * size.y() + point.y();
    }

    static inline Vector2u toPoint(unsigned int index, const Vector2u& size) {
        return {index / size.y(), index % size.y()};
    }

private:
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & _data;
        ar & _size;
    }
};


#endif //RGS_GRID_H
