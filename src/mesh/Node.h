#ifndef TESTING_NODE_H
#define TESTING_NODE_H

#include <utilities/Types.h>

class Node {
private:
    int _id;
    Vector3d _position;
public:
    Node(int id, Vector3d position) : _id(id), _position(std::move(position)) {
        // nothing
    }

    int getId() const {
        return _id;
    }

    const Vector3d& getPosition() const {
        return _position;
    }
};


#endif //TESTING_NODE_H
