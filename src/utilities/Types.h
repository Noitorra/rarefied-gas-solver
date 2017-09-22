#ifndef TYPES_H_
#define TYPES_H_

#include <vector>
#include <cmath>
#include <ostream>
#include <boost/serialization/access.hpp>
#include <boost/serialization/vector.hpp>

/*
 * This class is a base class with basic method,
 * working with inner vector only
 */
template<typename T>
class VectorBase {
    friend class boost::serialization::access;

public:
    VectorBase() {}

    virtual ~VectorBase() {
        _array.clear();
    }

    std::vector<T>& getMass() {
        return _array;
    }

    const T mod() const {
        return std::sqrt(mod2());
    }

    const T mod2() const {
        return calcMod2();
    }

    const T& get(unsigned int i) const {
        return _array[i];
    }

    T& operator[](unsigned int i) {
        return _array[i];
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

protected:
    std::vector<T> _array;

private:
    const T calcMod2() const {
        T loc_mod2 = T(0);
        for (auto& item : _array) {
            loc_mod2 += (item * item);
        }
        return loc_mod2;
    }

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & _array;
    }
};

// public class, which contains x, y access
// and set method..
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

    const Vector2& operator-=(const Vector2& right) {
        for (unsigned int i = 0; i < this->_array.size(); i++) {
            this->_array[i] -= right._array[i];
        }
        return *this;
    }

    const Vector2& operator/=(const double val) {
        for (unsigned int i = 0; i < this->_array.size(); i++) {
            this->_array[i] /= val;
        }
        return *this;
    }

    const Vector2& operator*=(const double val) {
        for (unsigned int i = 0; i < this->_array.size(); i++) {
            this->_array[i] *= val;
        }
        return *this;
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
};

typedef Vector2<double> Vector2d;
typedef Vector2<float> Vector2f;
typedef Vector2<int> Vector2i;
typedef Vector2<unsigned int> Vector2u;
typedef Vector2<short> Vector2b;

// public class, which contains x, y access
// and set method..
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

    inline bool operator==(const Vector3& rhs) const {
        bool equal = true;
        for (unsigned int i = 0; i < this->_array.size(); i++) {
            equal = this->_array[i] == rhs._array[i] && equal;
        }
        return equal;
    }
};

typedef Vector3<double> Vector3d;
typedef Vector3<float> Vector3f;
typedef Vector3<int> Vector3i;
typedef Vector3<unsigned int> Vector3u;
typedef Vector3<short> Vector3b;

#endif // TYPES_H_
