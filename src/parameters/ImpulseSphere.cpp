#include <cmath>
#include <vector>
#include <iostream>

#include "ImpulseSphere.h"
#include "core/Config.h"

ImpulseSphere::ImpulseSphere(double maxImpulse, unsigned int resolution) : _maxImpulse(maxImpulse), _resolution(resolution) {}

ImpulseSphere::~ImpulseSphere() {
    if (_xyz2i != nullptr) {
        for (unsigned int x = 0; x < _resolution; x++) {
            for (unsigned int y = 0; y < _resolution; y++) {
                delete[] _xyz2i[x][y];
            }
            delete[] _xyz2i[x];
        }
        delete[] _xyz2i;
    }
}

void ImpulseSphere::init() {

    // calc delta impulse
    _deltaImpulse = 2 * _maxImpulse / _resolution;
    _deltaImpulseQube = std::pow(_deltaImpulse, 3);

    // calc line impulse vector
    std::vector<double> lineImpulse;
    for (unsigned int i = 0; i < _resolution; i++) {
        lineImpulse.push_back(_deltaImpulse * (i + 0.5) - _maxImpulse);
    }

    // creating impulse sphere
    _xyz2i = new int**[_resolution];
    for (auto x = 0; x < lineImpulse.size(); x++) {
        _xyz2i[x] = new int*[_resolution];
        for (auto y = 0; y < lineImpulse.size(); y++) {
            _xyz2i[x][y] = new int[_resolution];
            for (auto z = 0; z < lineImpulse.size(); z++) {
                Vector3d impulse = {lineImpulse[x], lineImpulse[y], lineImpulse[z]};
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

double ImpulseSphere::getMaxImpulse() const {
    return _maxImpulse;
}

unsigned int ImpulseSphere::getResolution() const {
    return _resolution;
}

int*** ImpulseSphere::getXYZ2I() const {
    return _xyz2i;
}

double ImpulseSphere::getDeltaImpulse() const {
    return _deltaImpulse;
}

double ImpulseSphere::getDeltaImpulseQube() const {
    return _deltaImpulseQube;
}

const std::vector<Vector3d>& ImpulseSphere::getImpulses() const {
    return _impulses;
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

//int ImpulseSphere::reverseIndex(int ii, unsigned int axis) {
//    // ii -> xyz
//    // xyz -> reverse xyz -> rii
//    Vector3i v3i = _i2xyz[ii];
//    int ri = 0;
//    switch (static_cast<Config::Axis>(axis)) {
//        case Config::Axis::X:
//            ri = _xyz2i[_resolution - 1 - v3i.x()][v3i.y()][v3i.z()];
//            break;
//
//        case Config::Axis::Y:
//            ri = _xyz2i[v3i.x()][_resolution - 1 - v3i.y()][v3i.z()];
//            break;
//
//        case Config::Axis::Z:
//            ri = _xyz2i[v3i.x()][v3i.y()][_resolution - 1 - v3i.z()];
//            break;
//    }
//    if (ri == -1) {
//        std::cout << "ImpulseSphere::reverseIndex() bad reverse, index = -1." << std::endl;
//        return ii;
//    }
//    //std::cout << "In: " << ii << " Reverse: " << ri << std::endl;
//    return ri;
//}
