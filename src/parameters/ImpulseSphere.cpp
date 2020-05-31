#include <cmath>
#include <vector>
#include <iostream>
#include <stdexcept>

#include "ImpulseSphere.h"
#include "core/Config.h"

void ImpulseSphere::init() {

    // calc delta impulse
    _deltaImpulse = 2 * _maxImpulse / _resolution;
    _deltaImpulseQube = std::pow(_deltaImpulse, 3);

    // calc line impulse vector
    std::vector<double> lineImpulses;
    for (unsigned int i = 0; i < _resolution; i++) {
        lineImpulses.push_back(_deltaImpulse * (i + 0.5) - _maxImpulse);
    }

    // creating impulse sphere
    _xyz2i = new int**[_resolution];
    for (auto x = 0; x < lineImpulses.size(); x++) {
        _xyz2i[x] = new int*[_resolution];
        for (auto y = 0; y < lineImpulses.size(); y++) {
            _xyz2i[x][y] = new int[_resolution];
            for (auto z = 0; z < lineImpulses.size(); z++) {
                Vector3d impulse = {lineImpulses[x], lineImpulses[y], lineImpulses[z]};
                if (impulse.module() < _maxImpulse) {
                    _impulses.push_back(impulse);
                    _i2xyz.emplace_back(x, y, z);
                    _xyz2i[x][y][z] = static_cast<int>(_i2xyz.size() - 1);
                } else {
                    _xyz2i[x][y][z] = -1;
                }
            }
        }
    }
}

int ImpulseSphere::reverseIndex(int ii, const Vector3d& normal) {
    Vector3d impulse = _impulses[ii];
    Vector3d reverseImpulse = impulse - normal * impulse.scalar(normal) * 2;

    int x = int((reverseImpulse.x() + _maxImpulse) / _deltaImpulse);
    int y = int((reverseImpulse.y() + _maxImpulse) / _deltaImpulse);
    int z = int((reverseImpulse.z() + _maxImpulse) / _deltaImpulse);

    if (x < 0 || x > _resolution - 1 || y < 0 || y > _resolution - 1 || z < 0 || z > _resolution - 1) {
        throw std::runtime_error("impulse index couldn't be reversed");
    }

    return _xyz2i[x][y][z];
}

std::ostream& operator<<(std::ostream& os, const ImpulseSphere& impulse) {
    os << "{";
    os << "MaxImpulse = " << impulse._maxImpulse << "; "
       << "Resolution = " << impulse._resolution << "; "
       << "DeltaImpulse = " << impulse._deltaImpulse << "; "
       << "DeltaImpulseQube = " << impulse._deltaImpulseQube;
    os << "}";
    return os;
}
