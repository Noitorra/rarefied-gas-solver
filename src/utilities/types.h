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

  std::vector<T>& getMass() { return m_vMass; }

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
			// loc_mod2 += (item*item);
      loc_mod2 += (item*item);
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
		this->m_vMass.resize(2, T(0));
	}
	Vector2(const T& x, const T& y) {
		this->m_vMass.resize(2, T(0));
		set(x, y);
	}

	void set(const T& x, const T& y) {
		this->m_vMass[0] = x;
		this->m_vMass[1] = y;
	}

	const Vector2 operator+(const Vector2& right) const {
		Vector2 v2;
		for(unsigned int i=0;i<this->m_vMass.size();i++) {
			v2.m_vMass[i] = this->m_vMass[i] + right.m_vMass[i];
		}
		return v2;
	}
  
	const Vector2& operator+=(const Vector2& right) {
		for(unsigned int i=0;i<this->m_vMass.size();i++) {
			this->m_vMass[i] += right.m_vMass[i];
		}
		return *this;
	}
  
	const Vector2 operator-(const Vector2& right) const {
		Vector2 v2;
		for(unsigned int i=0;i<this->m_vMass.size();i++) {
			v2.m_vMass[i] = this->m_vMass[i] - right.m_vMass[i];
		}
		return v2;
	}
  
	const Vector2& operator-=(const Vector2& right) {
		for(unsigned int i=0;i<this->m_vMass.size();i++) {
			this->m_vMass[i] -= right.m_vMass[i];
		}
		return *this;
	}

	const Vector2& operator/=(const double val) {
		for(unsigned int i=0;i<this->m_vMass.size();i++) {
			this->m_vMass[i] /= val;
		}
		return *this;
	}

	const Vector2& operator*=(const double val) {
		for(unsigned int i=0;i<this->m_vMass.size();i++) {
			this->m_vMass[i] *= val;
		}
		return *this;
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
		this->m_vMass.resize(3, T(0));
	}
	Vector3(const T& x, const T& y, const T& z) {
		this->m_vMass.resize(3, T(0));
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

	const Vector3 operator+(const Vector3& right) const {
		Vector3 v3;
		for(unsigned int i=0;i<this->m_vMass.size();i++) {
			v3.m_vMass[i] = this->m_vMass[i] + right.m_vMass[i];
		}
		return v3;
	}

	const Vector3& operator+=(const Vector3& right) {
		for(unsigned int i=0;i<this->m_vMass.size();i++) {
			this->m_vMass[i] += right.m_vMass[i];
		}
		return *this;
	}

	const Vector3 operator-(const Vector3& right) const {
		Vector3 v3;
		for(unsigned int i=0;i<this->m_vMass.size();i++) {
			v3.m_vMass[i] = this->m_vMass[i] - right.m_vMass[i];
		}
		return v3;
	}

	const Vector3& operator-=(const Vector3& right) {
		for(unsigned int i=0;i<this->m_vMass.size();i++) {
			this->m_vMass[i] += right.m_vMass[i];
		}
		return *this;
	}

	template<typename TValue>
	const Vector3 operator/(const TValue& right) const {
		Vector3 v3;
		for(unsigned int i=0;i<this->m_vMass.size();i++) {
			v3.m_vMass[i] = this->m_vMass[i] / right;
		}
		return v3;
	}

  template<typename TValue>
  const Vector3& operator/=(const TValue& right) {
    for (unsigned int i = 0; i<this->m_vMass.size(); i++) {
      this->m_vMass[i] /= right;
    }
    return *this;
  }

	template<typename TValue>
	const Vector3 operator*(const TValue& right) const {
		Vector3 v3;
		for(unsigned int i=0;i<this->m_vMass.size();i++) {
			v3.m_vMass[i] = this->m_vMass[i] * right;
		}
		return v3;
	}

	template<typename TValue>
	const Vector3& operator*=(const TValue& right) {
		for(unsigned int i=0;i<this->m_vMass.size();i++) {
			this->m_vMass[i] *= right;
		}
		return *this;
	}

	inline bool operator==(const Vector3& rhs) const {
    bool bEqual = true;
		for(unsigned int i=0;i<this->m_vMass.size();i++) {
			bEqual = this->m_vMass[i] == rhs.m_vMass[i] && bEqual;
		}
		return bEqual;
	}
};

typedef Vector3<double> 			 Vector3d;
typedef Vector3<float> 			 Vector3f;
typedef Vector3<int> 				 Vector3i;
typedef Vector3<unsigned int> Vector3u;
// with bool this doesn't compliled?!
typedef Vector3<int> Vector3b;

#endif // TYPES_H_