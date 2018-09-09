#ifndef RGS_NODE_H
#define RGS_NODE_H

#include <utilities/Types.h>

class Node {
    friend class boost::serialization::access;

private:
    int _id;
    Vector3d _position;

public:
    Node() = default;
    Node(const Node&) = default;

    Node(int id, Vector3d position) : _id(id), _position(std::move(position)) {}

    int getId() const {
        return _id;
    }

    const Vector3d& getPosition() const {
        return _position;
    }

private:
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & _id;
        ar & _position;
    }
};


#endif //RGS_NODE_H
