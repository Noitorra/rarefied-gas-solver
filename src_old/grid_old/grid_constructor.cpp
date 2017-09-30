#include "grid_constructor.h"
#include "utilities/Config.h"
#include "utilities/Normalizer.h"
#include "parameters/BetaChain.h"
#include "emitter.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846  /* pi */
#endif

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

    // TODO: Remove?
    PushTemperature(1.0);
    PushPressure(1.0);

    //Vector2d vPhysSize = Vector2d(520.0, 0.4);
    Vector2d vPhysSize = Vector2d(5.2, 0.4);
    Vector2i vNumSize = Vector2i(100, 30);
    Config::getInstance()->setCellSize(Vector2d(vPhysSize.x() / vNumSize.x(), vPhysSize.y() / vNumSize.y()));

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

    AddBox(Vector2d(0.0, 0.0), vPhysSize, [&](int x, int y, GasesConfigsMap& configs, const Vector2i& size, const Vector2i& start) {

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
    });
}


void GridConstructor::ConfigureTestGrid() {

    const Config* pConfig = Config::getInstance();
    const Normalizer* pNormalizer = pConfig->getNormalizer();

    // Some beta chain hack. Makes lambda without data type.
    for (auto& item : pConfig->getBetaChains()) {
        pNormalizer->normalize(item->dLambda1, Normalizer::Type::LAMBDA);
        pNormalizer->normalize(item->dLambda2, Normalizer::Type::LAMBDA);
    }

    // TODO: Remove?
    PushTemperature(1.0);
    PushPressure(1.0);

    //Vector2d vPhysSize = Vector2d(520.0, 0.4);
    Vector2d vPhysSize = Vector2d(0.4, 0.4);
    Vector2i vNumSize = Vector2i(20, 20);
    Config::getInstance()->setCellSize(Vector2d(vPhysSize.x() / vNumSize.x(), vPhysSize.y() / vNumSize.y()));

    AddBox(Vector2d(0.0, 0.0), vPhysSize, [&](int x, int y, GasesConfigsMap& configs, const Vector2i& size, const Vector2i& start) {

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
    });
}
