#include "impulse.h"

Impulse::Impulse() {
    m_dMaxImpulse = 4.8;
    m_uResolution = 20;

    m_dDeltaImpulse = 0.0;
    m_dDeltaImpulseQube = 0.0;

    m_pxyz2i = nullptr;
}

Impulse::~Impulse() {
    if (m_pxyz2i != nullptr) {
        for (unsigned int x = 0; x < m_uResolution; x++) {
            for (unsigned int y = 0; y < m_uResolution; y++) {
                delete[] m_pxyz2i[x][y];
            }
            delete[] m_pxyz2i[x];
        }
        delete[] m_pxyz2i;
    }
}

void Impulse::Init() {

    // TODO: Possibly very wrong!
    /*
    GasVector& gasv = m_pSolver->GetGas();
    if (gasv.size() >= 2) {
    m_dMaxImpulse = std::max(gasv[0]->getMass(), gasv[1]->getMass()) * m_dMaxImpulse;
    std::cout << "Impulse::init() : gasv.size() >= 2" << std::endl;
    }
    else {
    std::cout << "Impulse::init() : gasv.size() < 2" << std::endl;
    }
    */

//    std::cout << "Impulse::init() : m_dMaxImpulse = " << m_dMaxImpulse << std::endl;

    // calc delta impulse
    m_dDeltaImpulse = 2 * m_dMaxImpulse / (m_uResolution);
    m_dDeltaImpulseQube = std::pow(m_dDeltaImpulse, 3);

    // calc line impulse vector
    std::vector<double> line;
    for (unsigned int i = 0; i < m_uResolution; i++) {
        line.push_back(m_dDeltaImpulse * (i + 0.5) - m_dMaxImpulse);
        //    std::cout << "[Impulse] impulse[" << i << "] = " << line.back() << std::endl;
    }

    // xyz2i
    m_pxyz2i = new int** [m_uResolution];

    // creating impulse sphere
    for (unsigned int x = 0; x < line.size(); x++) {
        m_pxyz2i[x] = new int* [m_uResolution];
        for (unsigned int y = 0; y < line.size(); y++) {
            m_pxyz2i[x][y] = new int[m_uResolution];
            for (unsigned int z = 0; z < line.size(); z++) {
                Vector3d vec(line[x], line[y], line[z]);
                if (vec.mod() < m_dMaxImpulse) {

                    // save index for xyz
                    m_pxyz2i[x][y][z] = static_cast<int>(m_vImpulse.size());

                    // save xyz for index
                    m_pi2xyz.push_back(Vector3i(x, y, z));

                    // save impulse value for index
                    m_vImpulse.push_back(vec);
                } else {
                    m_pxyz2i[x][y][z] = -1; // TODO: the fuck should i do here ... ?
                }
            }
        }
    }
}

void Impulse::setMaxImpulse(double max_impulse) {
    m_dMaxImpulse = max_impulse;
}

void Impulse::setResolution(unsigned int resolution) {
    m_uResolution = resolution;
}

unsigned int Impulse::getResolution() {
    return m_uResolution;
}

double Impulse::getMaxImpulse() {
    return m_dMaxImpulse;
}

double Impulse::getDeltaImpulse() {
    return m_dDeltaImpulse;
}

double Impulse::getDeltaImpulseQube() {
    return m_dDeltaImpulseQube;
}

std::vector<Vector3d>& Impulse::getVector() {
    return m_vImpulse;
}

int Impulse::reverseIndex(int ii, sep::Axis eAxis) {
    // ii -> xyz
    // xyz -> reverse xyz -> rii
    Vector3i v3i = m_pi2xyz[ii];
    int ri = 0;
    switch (eAxis) {
        case sep::X:
            ri = m_pxyz2i[m_uResolution - 1 - v3i.x()][v3i.y()][v3i.z()];
            break;
        case sep::Y:
            ri = m_pxyz2i[v3i.x()][m_uResolution - 1 - v3i.y()][v3i.z()];
            break;
        case sep::Z:
            ri = m_pxyz2i[v3i.x()][v3i.y()][m_uResolution - 1 - v3i.z()];
            break;
        default:
            break;
    }
    if (ri == -1) {
        std::cout << "Impulse::reverseIndex() bad reverse, index = -1." << std::endl;
        return ii;
    }
    //std::cout << "In: " << ii << " Reverse: " << ri << std::endl;
    return ri;
}
