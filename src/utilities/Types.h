#ifndef TYPES_H
#define TYPES_H

#include <vector>
#include <cmath>
#include <ostream>

#include <boost/serialization/access.hpp>
#include <boost/serialization/vector.hpp>

template<typename T>
class VectorBase {
public:
    friend class boost::serialization::access;

protected:
    std::vector<T> _array;

public:
    VectorBase() = default;

    std::vector<T>& getArray() {
        return _array;
    }

    const T module() const {
        return std::sqrt(moduleSquare());
    }

    const T moduleSquare() const {
        T moduleSquare = T(0);
        for (const auto& item : _array) {
            moduleSquare += (item * item);
        }
        return moduleSquare;
    }

    void normalizeSelf() {
        T m = module();
        for (auto& item : _array) {
            item /= m;
        }
    }

    const T& get(unsigned int i) const {
        return _array[i];
    }

    T& operator[](unsigned int i) {
        return _array[i];
    }

    bool isNull() const {
        for (const auto& item : _array) {
            if (item != T(0)) {
                return false;
            }
        }
        return true;
    }

    friend std::ostream &operator<<(std::ostream &os, const VectorBase &base) {
        os << "[";
        for (std::size_t i = 0; i < base._array.size(); i++) {
            os << base._array[i];
            if (i != base._array.size() - 1) {
                os << ", ";
            }
        }
        os << "]";
        return os;
    }

private:
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & _array;
    }
};

template<typename T>
class Vector2 : public VectorBase<T> {
public:
    Vector2() {
        this->_array.resize(2, T(0));
    }

    Vector2(const T& x, const T& y) {
        this->_array.resize(2, T(0));
        set(x, y);
    }

    void set(const T& x, const T& y) {
        this->_array[0] = x;
        this->_array[1] = y;
    }

    T& x() {
        return this->_array[0];
    }

    T& y() {
        return this->_array[1];
    }

    const T& x() const {
        return this->_array[0];
    }

    const T& y() const {
        return this->_array[1];
    }

    Vector2 normalize() {
        this->normalizeSelf();
        return &this;
    }

    const T scalar(const Vector2& right) const {
        T value = 0;
        for (unsigned int i = 0; i < this->_array.size(); i++) {
            value += this->_array[i] * right._array[i];
        }
        return value;
    }

    const Vector2 operator+(const Vector2& right) const {
        Vector2 v2;
        for (unsigned int i = 0; i < this->_array.size(); i++) {
            v2._array[i] = this->_array[i] + right._array[i];
        }
        return v2;
    }

    const Vector2& operator+=(const Vector2& right) {
        for (unsigned int i = 0; i < this->_array.size(); i++) {
            this->_array[i] += right._array[i];
        }
        return *this;
    }

    const Vector2 operator-(const Vector2& right) const {
        Vector2 v2;
        for (unsigned int i = 0; i < this->_array.size(); i++) {
            v2._array[i] = this->_array[i] - right._array[i];
        }
        return v2;
    }

    const Vector2 operator-() const {
        Vector2 v2;
        for (unsigned int i = 0; i < this->_array.size(); i++) {
            v2._array[i] = -this->_array[i];
        }
        return v2;
    }

    const Vector2& operator-=(const Vector2& right) {
        for (unsigned int i = 0; i < this->_array.size(); i++) {
            this->_array[i] -= right._array[i];
        }
        return *this;
    }

    template<typename TValue>
    const Vector2& operator/=(const TValue& val) {
        for (unsigned int i = 0; i < this->_array.size(); i++) {
            this->_array[i] /= val;
        }
        return *this;
    }

    template<typename TValue>
    const Vector2& operator*=(const TValue& val) {
        for (unsigned int i = 0; i < this->_array.size(); i++) {
            this->_array[i] *= val;
        }
        return *this;
    }

    bool operator==(const Vector2& rhs) const {
        bool isEqual = true;
        for (unsigned int i = 0; i < this->_array.size(); i++) {
            if (this->_array[i] != rhs._array[i]) {
                isEqual = false;
                break;
            }
        }
        return isEqual;
    }
};

typedef Vector2<double> Vector2d;
typedef Vector2<float> Vector2f;
typedef Vector2<int> Vector2i;
typedef Vector2<unsigned int> Vector2u;
typedef Vector2<short> Vector2b;

template<typename T>
class Vector3 : public VectorBase<T> {
public:
    Vector3() {
        this->_array.resize(3, T(0));
    }

    Vector3(const T& x, const T& y, const T& z) {
        this->_array.resize(3, T(0));
        set(x, y, z);
    }

    void set(const T& x, const T& y, const T& z) {
        this->_array[0] = x;
        this->_array[1] = y;
        this->_array[2] = z;
    }

    T& x() {
        return this->_array[0];
    }

    T& y() {
        return this->_array[1];
    }

    T& z() {
        return this->_array[2];
    }

    const T& x() const {
        return this->_array[0];
    }

    const T& y() const {
        return this->_array[1];
    }

    const T& z() const {
        return this->_array[2];
    }

    const Vector3& normalize() {
        this->normalizeSelf();
        return *this;
    }

    const T scalar(const Vector3& right) const {
        T value = 0;
        for (unsigned int i = 0; i < this->_array.size(); i++) {
            value += this->_array[i] * right._array[i];
        }
        return value;
    }

    Vector3 vector(const Vector3& right) const {
        return Vector3(
                y() * right.z() - z() * right.y(),
                z() * right.x() - x() * right.z(),
                x() * right.y() - y() * right.x()
        );
    }

    const Vector3 operator+(const Vector3& right) const {
        Vector3 v3;
        for (unsigned int i = 0; i < this->_array.size(); i++) {
            v3._array[i] = this->_array[i] + right._array[i];
        }
        return v3;
    }

    const Vector3& operator+=(const Vector3& right) {
        for (unsigned int i = 0; i < this->_array.size(); i++) {
            this->_array[i] += right._array[i];
        }
        return *this;
    }

    const Vector3 operator-(const Vector3& right) const {
        Vector3 v3;
        for (unsigned int i = 0; i < this->_array.size(); i++) {
            v3._array[i] = this->_array[i] - right._array[i];
        }
        return v3;
    }

    const Vector3 operator-() const {
        Vector3 v3;
        for (unsigned int i = 0; i < this->_array.size(); i++) {
            v3._array[i] = -this->_array[i];
        }
        return v3;
    }

    const Vector3& operator-=(const Vector3& right) {
        for (unsigned int i = 0; i < this->_array.size(); i++) {
            this->_array[i] -= right._array[i];
        }
        return *this;
    }

    template<typename TValue>
    const Vector3 operator/(const TValue& right) const {
        Vector3 v3;
        for (unsigned int i = 0; i < this->_array.size(); i++) {
            v3._array[i] = this->_array[i] / right;
        }
        return v3;
    }

    template<typename TValue>
    const Vector3& operator/=(const TValue& right) {
        for (unsigned int i = 0; i < this->_array.size(); i++) {
            this->_array[i] /= right;
        }
        return *this;
    }

    template<typename TValue>
    const Vector3 operator*(const TValue& right) const {
        Vector3 v3;
        for (unsigned int i = 0; i < this->_array.size(); i++) {
            v3._array[i] = this->_array[i] * right;
        }
        return v3;
    }

    template<typename TValue>
    const Vector3& operator*=(const TValue& right) {
        for (unsigned int i = 0; i < this->_array.size(); i++) {
            this->_array[i] *= right;
        }
        return *this;
    }

    bool operator==(const Vector3& rhs) const {
        bool isEqual = true;
        for (unsigned int i = 0; i < this->_array.size(); i++) {
            if (this->_array[i] != rhs._array[i]) {
                isEqual = false;
                break;
            }
        }
        return isEqual;
    }
};

typedef Vector3<double> Vector3d;
typedef Vector3<float> Vector3f;
typedef Vector3<int> Vector3i;
typedef Vector3<unsigned int> Vector3u;
typedef Vector3<short> Vector3b;

#endif // TYPES_H
