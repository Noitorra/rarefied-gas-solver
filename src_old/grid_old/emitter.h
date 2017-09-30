//
// Created by Sherbakov Dmitry on 14.08.16.
//

#ifndef RGS_EMITTER_H
#define RGS_EMITTER_H

#include <vector>
#include <string>

struct EmitterPart {
    EmitterPart();

    EmitterPart(double length, bool isAHole, double temp);

    void set(double length, bool isAHole, double temp);

    double length;
    bool isAHole;
    double temp;
};

class Emitter {
public:
    static Emitter* getInstance() {
        static Emitter* pInstance = new Emitter();
        return pInstance;
    }

    void fillEmitter();

    const double getLength() const;

    EmitterPart* getEmitterPart(double dBegin, double dEnd);

private:
    Emitter() {
        m_pEmitterPart = new EmitterPart();
        m_sFilename = "../resources/Emmiter_Temperature.txt";
    }

    void calc_length();

    std::vector<EmitterPart*> m_vEmitterParts;
    double m_dLength;
    EmitterPart* m_pEmitterPart;
    std::string m_sFilename;
};


#endif //RGS_EMITTER_H
