#include "grid_constructor.h"
#include "config.h"
#include "normalizer.h"
#include "parameters/beta_chain.h"

struct EmitterPart {
    EmitterPart() {
        set(0.0, false, 0.0);
    }

    EmitterPart(double length, bool isAHole, double temp) {
        set(length, isAHole, temp);
    }

    void set(double length, bool isAHole, double temp) {
        this->length = length;
        this->isAHole = isAHole;
        this->temp = temp;
    }

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

    void fillEmitter() {
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

        _calc_length();
    }

    const double getLength() const {
        return m_dLength;
    }

    EmitterPart* getEmitterPart(double dBegin, double dEnd) {
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

private:
    Emitter() {
        m_pEmitterPart = new EmitterPart();
        m_sFilename = "../resources/Emmiter_Temperature.txt";
    }

    void _calc_length() {
        m_dLength = 0.0;
        for (auto& item : m_vEmitterParts) {
            m_dLength += item->length;
        }
    }

    std::vector<EmitterPart*> m_vEmitterParts;
    double m_dLength;
    EmitterPart* m_pEmitterPart;
    std::string m_sFilename;
};

// This method being invoked while grid configuration
// Feel free to edit this method
void GridConstructor::ConfigureStandartGrid() {

    const Config* pConfig = Config::getInstance();
    const Normalizer* pNormalizer = pConfig->getNormalizer();

    // Some beta chain hack. Makes lambda without data type.
    for (auto& item : pConfig->getBetaChains()) {
        pNormalizer->normalize(item->dLambda1, Normalizer::Type::LAMBDA);
        pNormalizer->normalize(item->dLambda2, Normalizer::Type::LAMBDA);
    }

    // Bottom emmiter holes positioning
    Emitter::getInstance()->fillEmitter();
    std::cout << "Emmiter length = " << Emitter::getInstance()->getLength() << std::endl;

    PushTemperature(1.0);
    PushPressure(1.0);

    //Vector2d vPhysSize = Vector2d(520.0, 0.4);
    Vector2d vPhysSize = Vector2d(5.2, 0.4);
    Vector2i vNumSize = Vector2i(100, 30);
    Config::getInstance()->setCellSize(Vector2d(vPhysSize.x() / vNumSize.x(), vPhysSize.y() / vNumSize.y()));

    struct MyBox : GridBox {

        virtual void config(int x, int y, GasesConfigsMap& configs, const Vector2i& size, const Vector2i& start) {

            const Normalizer* pNormalizer = Config::getInstance()->getNormalizer();

            // Task parameters
            double T1 = pNormalizer->normalize(1500.0, Normalizer::Type::TEMPERATURE);
            double T2 = pNormalizer->normalize(900.0, Normalizer::Type::TEMPERATURE);

            double dPCsLeft = pNormalizer->normalize(150.0, Normalizer::Type::PRESSURE);
            double dPCsRight = pNormalizer->normalize(150.0, Normalizer::Type::PRESSURE);

            double dPKrStart = pNormalizer->normalize(7e-7, Normalizer::Type::PRESSURE);
            double dPXeStart = pNormalizer->normalize(1.2e-6, Normalizer::Type::PRESSURE);

            double dQKrFull = 2.1e-9;
            double dQXeFull = 3.6e-9;

            double dTFlow = 600.0;

            double dR = 4e-3;

            double dS = 2 * M_PI * dR * (10.8 * 7 + 11.8) * 1e-3;

            double dQKr = dQKrFull / (dTFlow * sep::BOLTZMANN_CONSTANT * dS);
            double dQXe = dQXeFull / (dTFlow * sep::BOLTZMANN_CONSTANT * dS);

            //std::cout << "FlowKr = " << dQKr << std::endl;
            //std::cout << "FlowXe = " << dQXe << std::endl;

            pNormalizer->normalize(dQKr, Normalizer::Type::FLOW);
            pNormalizer->normalize(dQXe, Normalizer::Type::FLOW);

            //std::cout << "FlowKr = " << dQKr << std::endl;
            //std::cout << "FlowXe = " << dQXe << std::endl;

            dQKr *= 1; // 5 10
            dQXe *= 1; // 5 10

            // support variables
            double dTgrad = T1 - (T1 - T2) * (2 * (y - 1) + 1) / 2 / (size.y() - 2);
            double dPgrad = dPCsLeft - (dPCsLeft - dPCsRight) * (2 * (x - 1) + 1) / 2 / (size.x() - 2);

            //configs[0].dPressure = dPCsLeft;
            configs[0].dPressure = dPgrad;
            configs[0].dTemperature = dTgrad;

            configs[1].dPressure = dPKrStart; // hack dPKrStart
            configs[1].dTemperature = dTgrad;

            configs[2].dPressure = dPXeStart; // hack
            configs[2].dTemperature = dTgrad;

            for (int i = 3; i < Config::getInstance()->getGasesNum(); i++) {
                configs[i].dPressure = 0.0;
                configs[i].dTemperature = dTgrad;
            }

            if (x == 0 && (y != 0 && y != size.y() - 1)) { // left border
                configs[0].eBoundaryType = sep::BT_GASE;
                configs[0].dBoundaryPressure = dPCsLeft;
                configs[0].dBoundaryTemperature = dTgrad;

                for (int i = 1; i < Config::getInstance()->getGasesNum(); i++) {
                    configs[i].eBoundaryType = sep::BT_DIFFUSE;
                    //configs[i].eBoundaryType = sep::BT_GASE;
                    configs[i].dBoundaryPressure = 0.0;
                    configs[i].vBoundaryFlow = Vector3d();
                    configs[i].dBoundaryTemperature = dTgrad;
                }
            }
            if (x == size.x() - 1 && (y != 0 && y != size.y() - 1)) { // right border
                configs[0].eBoundaryType = sep::BT_GASE;
                configs[0].dBoundaryPressure = dPCsRight; // dPCsRight
                configs[0].dBoundaryTemperature = dTgrad;

                for (int i = 1; i < Config::getInstance()->getGasesNum(); i++) {
                    configs[i].eBoundaryType = sep::BT_GASE;
                    configs[i].dBoundaryPressure = 0.0;
                    configs[i].vBoundaryFlow = Vector3d();
                    configs[i].dBoundaryTemperature = dTgrad;
                }
            }
            if (y == 0 && (x != 0 && x != size.x() - 1)) { // bottom border
                double dBegin = (1.0 * (x - 1)) / (size.x() - 2);
                double dEnd = (1.0 * (x)) / (size.x() - 2);
                EmitterPart* e = Emitter::getInstance()->getEmitterPart(dBegin, dEnd);
                if (e && e->isAHole) { // a hole
                    configs[0].eBoundaryType = sep::BT_DIFFUSE;
                    configs[0].dBoundaryTemperature = e->temp;

                    // Kr
                    configs[1].eBoundaryType = sep::BT_FLOW;
                    configs[1].vBoundaryFlow = Vector3d(0.0, dQKr, 0.0);
                    configs[1].dBoundaryTemperature = e->temp;

                    // Xe
                    configs[2].eBoundaryType = sep::BT_FLOW;
                    configs[2].vBoundaryFlow = Vector3d(0.0, dQXe, 0.0);
                    configs[2].dBoundaryTemperature = e->temp;

                    for (int i = 3; i < Config::getInstance()->getGasesNum(); i++) {
                        configs[i].eBoundaryType = sep::BT_DIFFUSE;
                        configs[i].dBoundaryTemperature = e->temp;
                    }
                } else {
                    configs[0].eBoundaryType = sep::BT_DIFFUSE;
                    configs[0].dBoundaryTemperature = e ? e->temp : T1;

                    for (int i = 1; i < Config::getInstance()->getGasesNum(); i++) {
                        configs[i].eBoundaryType = sep::BT_DIFFUSE;
                        configs[i].dBoundaryTemperature = e ? e->temp : T1;
                    }
                }
            }
            if (y == size.y() - 1 && (x != 0 && x != size.x() - 1)) { // top border
                configs[0].eBoundaryType = sep::BT_DIFFUSE;
                configs[0].dBoundaryTemperature = T2;

                for (int i = 1; i < Config::getInstance()->getGasesNum(); i++) {
                    configs[i].eBoundaryType = sep::BT_DIFFUSE;
                    configs[i].dBoundaryTemperature = T2;
                }
            }
        }
    };

    AddBox(Vector2d(0.0, 0.0), vPhysSize, new MyBox());
}


void GridConstructor::ConfigureTestGrid() {

    const Config* pConfig = Config::getInstance();
    const Normalizer* pNormalizer = pConfig->getNormalizer();

    // Some beta chain hack. Makes lambda without data type.
    for (auto& item : pConfig->getBetaChains()) {
        pNormalizer->normalize(item->dLambda1, Normalizer::Type::LAMBDA);
        pNormalizer->normalize(item->dLambda2, Normalizer::Type::LAMBDA);
    }

    PushTemperature(1.0);
    PushPressure(1.0);

    //Vector2d vPhysSize = Vector2d(520.0, 0.4);
    Vector2d vPhysSize = Vector2d(0.4, 0.4);
    Vector2i vNumSize = Vector2i(20, 20);
    Config::getInstance()->setCellSize(Vector2d(vPhysSize.x() / vNumSize.x(), vPhysSize.y() / vNumSize.y()));


    struct TestBox : GridBox {

        virtual void config(int x, int y, GasesConfigsMap& configs, const Vector2i& size, const Vector2i& start) {

            const Normalizer* pNormalizer = Config::getInstance()->getNormalizer();

            // Task parameters
            double T1 = pNormalizer->normalize(900.0, Normalizer::Type::TEMPERATURE);
            double T2 = pNormalizer->normalize(900.0, Normalizer::Type::TEMPERATURE);

            double dPCsLeft = pNormalizer->normalize(450.0, Normalizer::Type::PRESSURE);
            double dPCsRight = pNormalizer->normalize(150.0, Normalizer::Type::PRESSURE);

            double dFlowCsLeft = 0.1;
            double dFlowCsRight = 0.1;

            // support variables
            double dTgrad = T1 - (T1 - T2) * (2 * (y - 1) + 1) / 2 / (size.y() - 2);
            double dPgrad = dPCsLeft - (dPCsLeft - dPCsRight) * (2 * (x - 1) + 1) / 2 / (size.x() - 2);

            //dTgrad = (T1 + T2) / 2;
            dPgrad = (dPCsLeft + dPCsRight) / 2;

            //configs[0].dPressure = dPCsLeft;
            configs[0].dPressure = dPgrad; // dPgrad
            configs[0].dTemperature = dTgrad;

            if (x == 0) { // left border
                configs[0].eBoundaryType = sep::BT_GASE;
                configs[0].dBoundaryPressure = dPCsLeft;
                //configs[0].vBoundaryFlow = Vector3d(dFlowCsLeft, 0.0, 0.0);
                configs[0].dBoundaryTemperature = dTgrad;
            }
            if (x == size.x() - 1) { // right border
                configs[0].eBoundaryType = sep::BT_GASE;
                configs[0].dBoundaryPressure = dPCsRight; // dPCsRight
                configs[0].dBoundaryTemperature = dTgrad;
            }
            if (y == 0) { // bottom border
                configs[0].eBoundaryType = sep::BT_DIFFUSE;
                configs[0].dBoundaryTemperature = T1;
            }
            if (y == size.y() - 1) { // top border
                configs[0].eBoundaryType = sep::BT_DIFFUSE;
                configs[0].dBoundaryTemperature = T2;
            }
        }
    };

    AddBox(Vector2d(0.0, 0.0), vPhysSize, new TestBox());
}
