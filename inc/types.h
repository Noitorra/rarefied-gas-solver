#ifndef TYPES_H_
#define TYPES_H_

#include <vector>

template<typename T, unsigned int vec_dim>
class VectorBase {
public:
	VectorBase() {
		m_vMass.resize(vec_dim);
	}
	virtual ~VectorBase() {}


protected:
	std::vector<T> m_vMass;

};

#endif // TYPES_H_
