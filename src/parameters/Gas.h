#ifndef RGS_GAS_H
#define RGS_GAS_H

#include <ostream>

#include <boost/serialization/access.hpp>

class Gas {
    friend class boost::serialization::access;

private:
    double _mass;

public:
    Gas() = default;

    explicit Gas(double mass) {
        _mass = mass;
    }

    double getMass() const {
        return _mass;
    }

    friend std::ostream& operator<<(std::ostream& os, const Gas& gas) {
        os << "Mass = " << gas._mass;
        return os;
    }

private:
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & _mass;
    }

};

#endif /* RGS_GAS_H */
