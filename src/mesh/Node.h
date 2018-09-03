#ifndef RGS_NODE_H
#define RGS_NODE_H

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


#endif //RGS_NODE_H
