#ifndef RGS_GAS_H
#define RGS_GAS_H

#include <ostream>

#include <boost/serialization/access.hpp>

class Gas {
    friend class boost::serialization::access;

private:
    double _mass;
    double _radius;

public:
    Gas() = default;

    explicit Gas(double mass, double radius) {
        _mass = mass;
        _radius = radius;
    }

    double getMass() const {
        return _mass;
    }

    void setMass(double mass) {
        _mass = mass;
    }

    double getRadius() const {
        return _radius;
    }

    friend std::ostream& operator<<(std::ostream& os, const Gas& gas) {
        os << "Mass = " << gas._mass << " Radius = " << gas._radius;
        return os;
    }

private:
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & _mass;
        ar & _radius;
    }

};

#endif /* RGS_GAS_H */
