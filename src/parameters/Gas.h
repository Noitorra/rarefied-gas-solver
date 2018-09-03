#ifndef GAS_H
#define GAS_H

#include <ostream>

class Gas {
private:
    double _mass;
public:
    explicit Gas(double mass) {
        _mass = mass;
    }

    double getMass() const {
        return _mass;
    }

    friend std::ostream& operator<<(std::ostream& os, const Gas& gas) {
        os << "Mass: " << gas._mass;
        return os;
    }
};

#endif /* GAS_H */
