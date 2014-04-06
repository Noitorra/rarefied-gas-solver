#ifndef TYPES_H_
#define TYPES_H_

#include <vector>
#include <cmath>

// This class is a base class with basic method,
// working with inner vector only
template<typename T>
class VectorBase {
public:
	VectorBase() {}
	virtual ~VectorBase() {
		m_vMass.clear();
	}

	const T mod() const {
		return std::sqrt(mod2());
	}

	const T mod2() const {
		return calc_mod2();
	}

	T& operator[](unsigned int i) {
		return m_vMass[i];
	}
private:
	const T calc_mod2() const {
		T loc_mod2 = T(0);
		for( auto& item : m_vMass) {
			loc_mod2 += item*item;
		}
		return loc_mod2;
	}
protected:
	std::vector<T> m_vMass;
};

// public class, which contains x, y access
// and set method..
template<typename T>
class Vector2 : public VectorBase<T> {
public:
	Vector2() {
		this->m_vMass.resize(2);
	}
	Vector2(const T& x, const T& y) : Vector2() {
		set(x, y);
	}

	void set(const T& x, const T& y) {
		this->m_vMass[0] = x;
		this->m_vMass[1] = y;
	}

	T& x() { return this->m_vMass[0]; }
	T& y() { return this->m_vMass[1]; }
	const T& x() const { return this->m_vMass[0]; }
	const T& y() const { return this->m_vMass[1]; }
};

typedef Vector2<double> 			 Vector2d;
typedef Vector2<float> 			 Vector2f;
typedef Vector2<int> 				 Vector2i;
typedef Vector2<unsigned int> Vector2u;

// public class, which contains x, y access
// and set method..
template<typename T>
class Vector3 : public VectorBase<T> {
public:
	Vector3() {
		this->m_vMass.resize(3);
	}
	Vector3(const T& x, const T& y, const T& z) : Vector3() {
		set(x, y, z);
	}

	void set(const T& x, const T& y, const T& z) {
		this->m_vMass[0] = x;
		this->m_vMass[1] = y;
		this->m_vMass[2] = z;
	}

	T& x() { return this->m_vMass[0]; }
	T& y() { return this->m_vMass[1]; }
	T& z() { return this->m_vMass[2]; }
	const T& x() const { return this->m_vMass[0]; }
	const T& y() const { return this->m_vMass[1]; }
	const T& z() const { return this->m_vMass[2]; }
};

typedef Vector3<double> 			 Vector3d;
typedef Vector3<float> 			 Vector3f;
typedef Vector3<int> 				 Vector3i;
typedef Vector3<unsigned int> Vector3u;

#endif // TYPES_H_
