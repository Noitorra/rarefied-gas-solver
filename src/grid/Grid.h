#ifndef RGS_GRID_H
#define RGS_GRID_H

#include "utilities/types.h"
#include <utility>
#include <vector>
#include <ostream>

class CellData;

class Cell;

template<class T>
class Grid {
private:
    std::vector<T*> _values;
    Vector2u _size;
public:
    Grid() = default;

    explicit Grid(Vector2u size) : _size(size) {
        _values.resize(size.x() * size.y(), nullptr);
    }

    Grid(std::vector<T*> data, Vector2u size) : _values(std::move(data)), _size(size) {
        _values.resize(size.x() * size.y(), nullptr);
    }

    void resize(Vector2i point, Vector2u size) {
        std::vector<T*> newData(size.x() * size.y(), nullptr);
        for (unsigned int x = 0; x < size.x(); x++) {
            for (unsigned int y = 0; y < size.y(); y++) {
                if ((int) x + point.x() >= 0 && (int) y + point.y() >= 0) {
                    newData[toIndex({x, y}, size)] = get(x + point.x(), y + point.y());
                }
            }
        }
        _values.clear();
        _values.insert(std::begin(_values), std::begin(newData), std::end(newData));
        _size = size;
    }

    T* get(unsigned int x, unsigned int y) const {
        return get(Vector2u(x, y));
    }

    T* get(const Vector2u& point) const {
        return _values[toIndex(point, _size)];
    }

    void set(unsigned int x, unsigned int y, T* value) {
        set(Vector2u(x, y), value);
    }

    void set(const Vector2u& point, T* value) {
        _values[toIndex(point, _size)] = value;
    }

    const Vector2u& getSize() const {
        return _size;
    }

    void setByIndex(unsigned int index, T* value) {
        _values[index] = value;
    }

    T* getByIndex(unsigned int index) const {
        return _values[index];
    }

    unsigned int getCount() const {
        return (unsigned int) _values.size();
    }

    unsigned int getCountNotNull() const {
        unsigned int countNotNull = 0;
        for (std::size_t i = 0; i < _values.size(); i++) {
            if (_values[i] != nullptr) {
                countNotNull++;
            }
        }
        return countNotNull;
    }

    const std::vector<T*>& getValues() const {
        return _values;
    }

    friend std::ostream& operator<<(std::ostream& os, const Grid<CellData>& grid);

    friend std::ostream& operator<<(std::ostream& os, const Grid<Cell>& grid);

    static inline unsigned int toIndex(const Vector2u& point, const Vector2u& size) {
        return point.x() * size.y() + point.y();
    }

    static inline Vector2u toPoint(unsigned int index, const Vector2u& size) {
        return {index / size.y(), index % size.y()};
    }

private:
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & _values;
        ar & _size;
    }
};


#endif //RGS_GRID_H
