//
// Created by Sherbakov Dmitry on 14.08.16.
//

#include <ios>
#include <fstream>
#include <iostream>
#include <config/normalizer.h>
#include <config/config.h>
#include "emitter.h"

EmitterPart::EmitterPart() {
    set(0.0, false, 0.0);
}

EmitterPart::EmitterPart(double length, bool isAHole, double temp) {
    set(length, isAHole, temp);
}

void EmitterPart::set(double length, bool isAHole, double temp) {
    this->length = length;
    this->isAHole = isAHole;
    this->temp = temp;
}

void Emitter::fillEmitter() {
    double hole_size = 10.8;

    std::ifstream filestream(m_sFilename.c_str(), std::ios_base::in);
    if (filestream.is_open()) {
        std::string str;
        bool dataReady = false;
        double last_coord = 0.0;
        double last_temp = 0.0;
        bool firstInLine = false;
        while (filestream.good()) {
            if (dataReady) {
                double coord, temp;
                if (filestream >> coord >> temp) {
                    // Good data, need to add to emitter
                    if (firstInLine) {
                        // first element in line
                        firstInLine = false;
                        if (!m_vEmitterParts.empty()) {
                            if (!m_vEmitterParts.back()->isAHole) {
                                std::cout << "EMITTER ERROR: no hole in back()!!!" << std::endl;
                            } else {
                                m_vEmitterParts.back()->temp = (last_temp + temp) / 2;
                            }
                        }
                        last_temp = temp;
                    }
                    if (coord > last_coord) {
                        m_vEmitterParts.push_back(new EmitterPart(coord - last_coord, false, (temp + last_temp) / 2));
                        last_coord = coord;
                        last_temp = temp;
                    }
                } else {
                    dataReady = false;
                    filestream.clear();
                    // Add hole
                    m_vEmitterParts.push_back(new EmitterPart(hole_size, true, 0.0));
                }

            } else {
                std::getline(filestream, str);
                if (str.find("Line") != std::string::npos) { // it is line here
                    dataReady = true;
                    firstInLine = true;
                }
            }
        }

        filestream.close();

        // get rid of last part!
        if (m_vEmitterParts.empty()) {
            std::cout << "EMITTER ERROR: Emitter is empty #01." << std::endl;
        } else {
            EmitterPart* part = m_vEmitterParts.back();
            if (part->isAHole) {
                m_vEmitterParts.pop_back();
            } else {
                std::cout << "EMITTER ERROR: last element is not a HOLE!!!" << std::endl;
            }
        }

        if (m_vEmitterParts.empty()) {
            std::cout << "EMITTER ERROR: Emitter is empty #02." << std::endl;
            for (auto iter = m_vEmitterParts.end(); iter != m_vEmitterParts.begin(); iter--) {
                (*iter)->length = 11.8;
            }
        }
    } else {
        std::cout << "EMITTER WARNING: Can't open file " << m_sFilename << std::endl;

        m_vEmitterParts.push_back(new EmitterPart(86, false, (1200.0 + 1475.0) / 2));
        m_vEmitterParts.push_back(new EmitterPart(10.8, true, (1475.0 + 1340.0) / 2));
        m_vEmitterParts.push_back(new EmitterPart(45.5, false, (1340.0 + 1480.0) / 2));
        m_vEmitterParts.push_back(new EmitterPart(10.8, true, (1480.0 + 1350.0) / 2));
        m_vEmitterParts.push_back(new EmitterPart(36.5, false, (1350.0 + 1480.0) / 2));
        m_vEmitterParts.push_back(new EmitterPart(10.8, true, (1480.0 + 1350.0) / 2));
        m_vEmitterParts.push_back(new EmitterPart(32.2, false, (1350.0 + 1460.0) / 2));
        m_vEmitterParts.push_back(new EmitterPart(10.8, true, (1460.0 + 1360.0) / 2));
        m_vEmitterParts.push_back(new EmitterPart(32.2, false, (1360.0 + 1480.0) / 2));
        m_vEmitterParts.push_back(new EmitterPart(10.8, true, (1480.0 + 1350.0) / 2));
        m_vEmitterParts.push_back(new EmitterPart(32.2, false, (1350.0 + 1460.0) / 2));
        m_vEmitterParts.push_back(new EmitterPart(10.8, true, (1460.0 + 1350.0) / 2));
        m_vEmitterParts.push_back(new EmitterPart(36.5, false, (1350.0 + 1475.0) / 2));
        m_vEmitterParts.push_back(new EmitterPart(10.8, true, (1475.0 + 1350.0) / 2));
        m_vEmitterParts.push_back(new EmitterPart(45.5, false, (1350.0 + 1365.0) / 2));
        m_vEmitterParts.push_back(new EmitterPart(11.8, true, (1365.0 + 1320.0) / 2));
        m_vEmitterParts.push_back(new EmitterPart(86.0, false, (1320.0 + 1400.0) / 2));
    }

    // Get Rid of Kelvin
    for (auto& item : m_vEmitterParts) {
        Config::getInstance()->getNormalizer()->normalize(item->temp, Normalizer::Type::TEMPERATURE);
    }

    calc_length();
}

const double Emitter::getLength() const {
    return m_dLength;
}

EmitterPart* Emitter::getEmitterPart(double dBegin, double dEnd) {
    // get all parts within beg end cell
    std::vector<EmitterPart*> parts;
    double l = 0.0; // already calc
    for (auto& item : m_vEmitterParts) {
        if (((dBegin + dEnd) / 2 * m_dLength > l && (dEnd + dBegin) / 2 * m_dLength <= l + item->length) ||
            ((l + item->length / 2) > dBegin * m_dLength && (l + item->length / 2) <= dEnd * m_dLength)) {
            parts.push_back(item);
        }
        l += item->length;
    }

    // no parts == logic error!!!
    if (parts.empty()) {
        std::cout << "EMITTER ERROR: Emitter can't find its part!!!" << std::endl;
        return nullptr;
    }

    // check if there is a hole
    m_pEmitterPart->isAHole = false;
    for (auto& item : parts) {
        if (item->isAHole) {
            m_pEmitterPart->isAHole = true;
            m_pEmitterPart->temp = item->temp;
        }
    }

    // if it is not a hole we need to average temperature.
    if (!m_pEmitterPart->isAHole) {
        m_pEmitterPart->temp = 0.0;
        for (auto& item : parts) {
            m_pEmitterPart->temp += item->temp;
        }
        m_pEmitterPart->temp /= parts.size();
    }

    return m_pEmitterPart;
}

void Emitter::calc_length() {
    m_dLength = 0.0;
    for (auto& item : m_vEmitterParts) {
        m_dLength += item->length;
    }
}