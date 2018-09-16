#ifndef RGS_PHYSICALENITY_H
#define RGS_PHYSICALENITY_H

#include <string>
#include <boost/serialization/access.hpp>
#include <boost/serialization/string.hpp>

class PhysicalEntity {
    friend class boost::serialization::access;

private:
    int _dimension;
    int _id;
    std::string _name;

public:
    PhysicalEntity() = default;

    PhysicalEntity(int dimension, int id, std::string name) : _dimension(dimension), _id(id), _name(std::move(name)) {}

    int getDimension() const {
        return _dimension;
    }

    int getId() const {
        return _id;
    }

    const std::string& getName() const {
        return _name;
    }

private:
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & _dimension;
        ar & _id;
        ar & _name;
    }
};

#endif //RGS_PHYSICALENITY_H
