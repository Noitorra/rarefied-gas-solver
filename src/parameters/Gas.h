#ifndef GAS_H_
#define GAS_H_

#include <ostream>

class Gas {
private:
    double m_dMass;
public:
    explicit Gas(double mass);

    double getMass() const {
        return m_dMass;
    }

    friend std::ostream& operator<<(std::ostream& os, const Gas& gas);
};

#endif /* GAS_H_ */
