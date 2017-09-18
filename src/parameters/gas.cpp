#include "gas.h"

Gas::Gas(double mass) {
    m_dMass = mass;
}

std::ostream& operator<<(std::ostream& os, const Gas& gas) {
    os << "Mass: " << gas.m_dMass;
    return os;
}
