#include "grid_constructor.h"
#include "utilities/Config.h"
#include "utilities/Normalizer.h"
#include "parameters/BetaChain.h"

void GridConstructor::ConfigureGPRT() {

    const Config* pConfig = Config::getInstance();
    const Normalizer* pNormalizer = pConfig->getNormalizer();

    Config::getInstance()->setCellSize(Vector2d(12.0, 0.4));

    double T1 = 325.0 + 273.0;
    double T2 = 60.0 + 273.0;
    Vector2d walls(Config::getInstance()->getCellSize());
    Vector2d sp_delta(Config::getInstance()->getCellSize());
    walls *= 2.0;

    pNormalizer->normalize(T1, Normalizer::Type::TEMPERATURE);
    pNormalizer->normalize(T2, Normalizer::Type::TEMPERATURE);

    PushTemperature(T1);

    double P_sat_T1 = 1.0; // 150Pa, T = T0, n = n0
    double P_sat_T2 = pNormalizer->normalize(2.7e-4, Normalizer::Type::PRESSURE); // 2.7 x 10^-4 Pa ��� 320 K
    double Q_Xe_in = pNormalizer->normalize(8.6e15, Normalizer::Type::FLOW); // 8.6 x 10^15 1/(m^2 * s)
    double P_Xe_in = pNormalizer->normalize(1.2e-6, Normalizer::Type::PRESSURE); // 1.2 X 10^-6 Pa
    double P_sat_Xe = P_Xe_in * 0.5;

    // Kr
    double Q_Kr_in = pNormalizer->normalize(5.0e15, Normalizer::Type::FLOW); // 5.0 x 10^15 1/(m^2 * s)
    double P_Kr_in = pNormalizer->normalize(7e-7, Normalizer::Type::PRESSURE); // 7 X 10^-7 Pa
    double P_sat_Kr = P_Kr_in * 0.5;

    double box_6_start_x = 330.0 - 3 * sp_delta.x();

    auto global_temp = [](Vector2d p, double& temperature, double box_6_start_x, double T1, double T2) -> void {
        //        std::cout << "T1 = " << T1 << " " << T2 << std::endl;
        const double sp_delta_wall = 30.0;
        const double temp_start = 100.0 + sp_delta_wall;
        const double temp_finish = box_6_start_x + 50.0;
        if (p.x() < temp_start) {
            temperature = T1;
            return;
        }
        if (p.x() > temp_finish) {
            temperature = T2;
            return;
        }
        temperature = (p.x() - temp_start) / (temp_finish - temp_start) * (T2 - T1) + T1;
        //std::cout << "T = " << temperature << std::endl;
    };

    auto global_pressure_Cs = [](Vector2d p, double& pressure, double P_sat_T1) -> void {
        pressure = (1.0 - (p.x() - 30.0) / 400.0) * P_sat_T1;
        //std::cout << "P = " << dPressure << std::endl;
    };

    auto global_pressure_Xe = [](Vector2d p, double& pressure, double P_sat_Xe) -> void {
        pressure = (1.0 - (p.x() - 30.0) / 400.0) * P_sat_Xe;
    };

    auto global_pressure_Kr = [](Vector2d p, double& pressure, double P_sat_Kr) -> void {
        pressure = (1.0 - (p.x() - 30.0) / 400.0) * P_sat_Kr;
    };

    auto gradient = [](double i, double max_i, double from, double to) -> double {
        return i / max_i * (to - from) + from;
    };

    // Some beta chain hack. Makes lambda without data type.
    for (auto& item : pConfig->getBetaChains()) {
        pNormalizer->normalize(item->dLambda1, Normalizer::Type::LAMBDA);
        pNormalizer->normalize(item->dLambda2, Normalizer::Type::LAMBDA);
    }

    // boxes ========================================================================
    // box 1
    //    PushPressure(1.0);
    PushPressure(P_sat_T1);
    AddBox(Vector2d(-20.0, 0.0), Vector2d(150.0 - sp_delta.x(), 4.0), [&](int x, int y, GasesConfigsMap& configs, const Vector2i& size, const Vector2i& start) {
        configs[1].dPressure = gradient(x, size.x() - 1, P_Xe_in, P_Xe_in + (P_sat_Xe - P_Xe_in) * 0.3);
        configs[2].dPressure = gradient(x, size.x() - 1, P_Kr_in, P_Kr_in + (P_sat_Kr - P_Kr_in) * 0.3);

        if (x == 0) {
            configs[0].eBoundaryType = sep::BT_GASE;
            configs[0].vBoundaryFlow = Vector3d();
            configs[0].dBoundaryPressure = P_sat_T1;
            configs[0].dBoundaryTemperature = T1;

            configs[1].eBoundaryType = sep::BT_GASE;
            configs[1].vBoundaryFlow = Vector3d(Q_Xe_in, 0.0, 0.0);
            configs[1].dBoundaryPressure = P_Xe_in;
            configs[1].dBoundaryTemperature = T1;

            configs[2].eBoundaryType = sep::BT_GASE;
            configs[2].vBoundaryFlow = Vector3d(Q_Kr_in, 0.0, 0.0);
            configs[2].dBoundaryPressure = P_Kr_in;
            configs[2].dBoundaryTemperature = T1;
        }

//               if (y == 0) {
//                   for (int gas = 0; gas < Config::m_iGasesNum; gas++) {
//                       configs[gas].eBoundaryType = sep::BT_MIRROR;
//                   }
//               }
    });

    // box 2
    //    PushPressure(0.95);
    AddBox(Vector2d(100.0, 0.0), Vector2d(30.0, 12.5), [&](int x, int y, GasesConfigsMap& configs, const Vector2i& size, const Vector2i& start) {
        configs[1].dPressure = gradient(y, size.y() - 1, P_Xe_in + (P_sat_Xe - P_Xe_in) * 0.3, P_Xe_in + (P_sat_Xe - P_Xe_in) * 0.6);
        configs[2].dPressure = gradient(y, size.y() - 1, P_Kr_in + (P_sat_Kr - P_Kr_in) * 0.3, P_Kr_in + (P_sat_Kr - P_Kr_in) * 0.6);

//        if (y == 0) {
//            for (int gas = 0; gas < Config::m_iGasesNum; gas++) {
//                configs[gas].eBoundaryType = sep::BT_MIRROR;
//            }
//        }
    });

    // box 3
    //    PushPressure(0.9);
    AddBox(Vector2d(30.0, 11.25), Vector2d(100.0, 1.25), [&](int x, int y, GasesConfigsMap& configs, const Vector2i& size, const Vector2i& start) {
        configs[1].dPressure = gradient(x, size.x() - 1, P_sat_Xe, P_Xe_in + (P_sat_Xe - P_Xe_in) * 0.6);
        configs[2].dPressure = gradient(x, size.x() - 1, P_sat_Kr, P_Kr_in + (P_sat_Kr - P_Kr_in) * 0.6);

        if (x == 0) {
            configs[0].eBoundaryType = sep::BT_GASE; // should be gas <-> fluid bound
            configs[0].dBoundaryPressure = P_sat_T1;
            configs[0].dBoundaryTemperature = T1;

            configs[1].eBoundaryType = sep::BT_GASE; // should be adsorption
            configs[1].dBoundaryPressure = P_sat_Xe;
            configs[1].dBoundaryTemperature = T1;

            configs[2].eBoundaryType = sep::BT_GASE; // should be adsorption
            configs[2].dBoundaryPressure = P_sat_Kr;
            configs[2].dBoundaryTemperature = T1;
        }
    });

    // box 4
    //    PushPressure(0.85);
    AddBox(Vector2d(30.0, 15.5), Vector2d(100.0 + walls.x(), 2.5), [&](int x, int y, GasesConfigsMap& configs, const Vector2i& size, const Vector2i& start) {
        Vector2i p_abs(x + start.x(), y + start.y());
        Vector2d p(p_abs.x() * Config::getInstance()->getCellSize().x(), p_abs.y() * pConfig->getCellSize().y());

        global_temp(p, configs[0].dTemperature, box_6_start_x, T1, T2);
        global_temp(p, configs[0].dBoundaryTemperature, box_6_start_x, T1, T2);
        global_pressure_Cs(p, configs[0].dPressure, P_sat_T1);

        global_temp(p, configs[1].dTemperature, box_6_start_x, T1, T2);
        global_temp(p, configs[1].dBoundaryTemperature, box_6_start_x, T1, T2);
        global_pressure_Xe(p, configs[1].dPressure, P_sat_Xe);

        global_temp(p, configs[2].dTemperature, box_6_start_x, T1, T2);
        global_temp(p, configs[2].dBoundaryTemperature, box_6_start_x, T1, T2);
        global_pressure_Kr(p, configs[2].dPressure, P_sat_Kr);

        if (x == 0) {
            configs[0].eBoundaryType = sep::BT_GASE; // should be fluid <-> gas bound
            configs[0].dBoundaryPressure = P_sat_T1;
            configs[0].dBoundaryTemperature = T1;

            configs[1].eBoundaryType = sep::BT_GASE; // should be adsorption
            configs[1].dBoundaryPressure = P_sat_Xe;
            configs[1].dBoundaryTemperature = T1;

            configs[2].eBoundaryType = sep::BT_GASE; // should be adsorption
            configs[2].dBoundaryPressure = P_sat_Kr;
            configs[2].dBoundaryTemperature = T1;
        }
    });

    // box 5
    //    PushPressure(0.8);
    AddBox(Vector2d(130.0 + walls.x()/* - sp_delta.x()*/, 0.0), Vector2d(200.0 - walls.x(), 18.0 - sp_delta.y()), [&](int x, int y, GasesConfigsMap& configs, const Vector2i& size, const Vector2i& start) {
        Vector2i p_abs(x + start.x(), y + start.y());
        Vector2d p(p_abs.x() * pConfig->getCellSize().x(), p_abs.y() * pConfig->getCellSize().y());

        global_temp(p, configs[0].dTemperature, box_6_start_x, T1, T2);
        global_temp(p, configs[0].dBoundaryTemperature, box_6_start_x, T1, T2);
        global_pressure_Cs(p, configs[0].dPressure, P_sat_T1);

        global_temp(p, configs[1].dTemperature, box_6_start_x, T1, T2);
        global_temp(p, configs[1].dBoundaryTemperature, box_6_start_x, T1, T2);
        global_pressure_Xe(p, configs[1].dPressure, P_sat_Xe);

        global_temp(p, configs[2].dTemperature, box_6_start_x, T1, T2);
        global_temp(p, configs[2].dBoundaryTemperature, box_6_start_x, T1, T2);
        global_pressure_Kr(p, configs[2].dPressure, P_sat_Kr);

//        if (y == 0) {
//            for (int gas = 0; gas < Config::m_iGasesNum; gas++) {
//                configs[gas].eBoundaryType = sep::BT_MIRROR;
//            }
//        }
    });

    // box 6
    //    PushPressure(0.75);
    AddBox(Vector2d(box_6_start_x, 0.0), Vector2d(100.0, 8.0), [&](int x, int y, GasesConfigsMap& configs, const Vector2i& size, const Vector2i& start) {
        Vector2i p_abs(x + start.x(), y + start.y());
        Vector2d p(p_abs.x() * pConfig->getCellSize().x(), p_abs.y() * pConfig->getCellSize().y());

        global_temp(p, configs[0].dTemperature, box_6_start_x, T1, T2);
        global_temp(p, configs[0].dBoundaryTemperature, box_6_start_x, T1, T2);
        global_pressure_Cs(p, configs[0].dPressure, P_sat_T1);

        global_temp(p, configs[1].dTemperature, box_6_start_x, T1, T2);
        global_temp(p, configs[1].dBoundaryTemperature, box_6_start_x, T1, T2);
        global_pressure_Xe(p, configs[1].dPressure, P_sat_Xe);

        global_temp(p, configs[2].dTemperature, box_6_start_x, T1, T2);
        global_temp(p, configs[2].dBoundaryTemperature, box_6_start_x, T1, T2);
        global_pressure_Kr(p, configs[2].dPressure, P_sat_Kr);

        if (x == size.x() - 1) {
            for (int gas = 0; gas < Config::getInstance()->getGasesNum(); gas++) {
                configs[gas].eBoundaryType = sep::BT_GASE;
                configs[gas].dBoundaryPressure = 0.0;
                configs[gas].dBoundaryTemperature = T2;
            }
        }

        //                if (y == 0) {
        //                    for (int gas = 0; gas < Config::m_iGasesNum; gas++) {
        //                        configs[gas].eBoundaryType = sep::BT_MIRROR;
        //                    }
        //                }

        // gas <-> fluid
        if (y == size.y() - 1 && p.x() > box_6_start_x + 50.0) {
            configs[0].eBoundaryType = sep::BT_GASE;
            configs[0].dBoundaryPressure = P_sat_T2;
            configs[0].dBoundaryTemperature = T2;
        }
    });

    // box 7
    //    PushPressure(0.7);
    AddBox(Vector2d(130.0/* - sp_delta.x()*/, 0.0), Vector2d(walls.x(), pConfig->getCellSize().y()), [&](int x, int y, GasesConfigsMap& configs, const Vector2i& size, const Vector2i& start) {
        if (x != 0 && x != size.x() - 1) {
            configs[0].iLockedAxes = sep::Y;
            configs[1].iLockedAxes = sep::Y;
            configs[2].iLockedAxes = sep::Y;
        }

        configs[0].dPressure = gradient(x, size.x() - 1, P_sat_T1, P_sat_T1 * 0.65);
        configs[1].dPressure = gradient(x, size.x() - 1, P_Xe_in + (P_sat_Xe - P_Xe_in) * 0.3, P_sat_Xe * 0.65);
        configs[2].dPressure = gradient(x, size.x() - 1, P_Kr_in + (P_sat_Kr - P_Kr_in) * 0.3, P_sat_Kr * 0.65);

        Vector2i p_abs(x + start.x(), y + start.y());
        Vector2d p(p_abs.x() * pConfig->getCellSize().x(), p_abs.y() * pConfig->getCellSize().y());

        global_temp(p, configs[0].dTemperature, box_6_start_x, T1, T2);
        global_temp(p, configs[0].dBoundaryTemperature, box_6_start_x, T1, T2);

        global_temp(p, configs[1].dTemperature, box_6_start_x, T1, T2);
        global_temp(p, configs[1].dBoundaryTemperature, box_6_start_x, T1, T2);

        global_temp(p, configs[2].dTemperature, box_6_start_x, T1, T2);
        global_temp(p, configs[2].dBoundaryTemperature, box_6_start_x, T1, T2);

        //                if (y == 0) {
        //                    for (int gas = 0; gas < Config::m_iGasesNum; gas++) {
        //                        configs[gas].eBoundaryType = sep::BT_MIRROR;
        //                    }
        //                }
    });
}

void GridConstructor::ConfigureGPRT2() {
    PushTemperature(0.5);
    AddBox(Vector2d(0.0, 0.0), Vector2d(10.0, 10.0), [](int x, int y, GasesConfigsMap& configs, const Vector2i& size, const Vector2i& start) {
        if (y == size.y() - 1) {
            configs[0].dBoundaryTemperature = 1.0;
            configs[1].dBoundaryTemperature = 1.0;
        }
        if (x == 0) {
            configs[0].eBoundaryType = sep::BT_MIRROR;
            configs[1].eBoundaryType = sep::BT_MIRROR;
        }
    });
}

void GridConstructor::BoundaryConditionTest() {

    /*
    *	Boundary conditions test
    */

    const Normalizer* pNormalizer = Config::getInstance()->getNormalizer();

    const double test_stream = pNormalizer->normalize(1.0e23, Normalizer::Type::FLOW); // 1e23 1/(m^2 * s)

    PushTemperature(1.0);
    PushPressure(1.0);

    AddBox(Vector2d(0.0, 0.0), Vector2d(10.0, 10.0), [&](int x, int y, GasesConfigsMap& configs, const Vector2i& size, const Vector2i& start) {
        configs[0].dPressure = 1.0;

        if (x == 0) {
            configs[0].eBoundaryType = sep::BT_GASE;
            configs[0].vBoundaryFlow = Vector3d(test_stream, 0.0, 0.0);
            configs[0].dBoundaryPressure = 1.0;
            configs[0].dBoundaryTemperature = 1.0;
        }

        if (x == size.x() - 1) {
            configs[0].eBoundaryType = sep::BT_GASE;
            configs[0].dBoundaryPressure = 1.0;
            configs[0].dBoundaryTemperature = 1.0;
        }
    });

    AddBox(Vector2d(12.0, 0.0), Vector2d(10.0, 10.0), [&](int x, int y, GasesConfigsMap& configs, const Vector2i& size, const Vector2i& start) {
        //configs[0].dPressure = 0.5;

        if (x == 0) {
            configs[0].eBoundaryType = sep::BT_GASE;
            configs[0].dBoundaryPressure = 1.0;
            configs[0].dBoundaryTemperature = 1.0;
        }
    });
}

void GridConstructor::PressureBoundaryConditionTestSmallArea() {

    /*
    *	Pressure boundary conditions test small area
    */

    Config::getInstance()->setCellSize(Vector2d(0.1, 0.002)); // mm

    PushTemperature(1.0);
    PushPressure(1.0);

    AddBox(Vector2d(0.0, 0.0), Vector2d(5.0, 0.1), [&](int x, int y, GasesConfigsMap& configs, const Vector2i& size, const Vector2i& start) {
        auto temp = [&](int x, int y, double& temperature) {
            temperature = ((double) y) / size.y() * (2.0 - 1.0) + 1.0;
        };

        if (x == 0) {
            temp(x, y, configs[0].dBoundaryTemperature);
        }

        if (x == size.x() - 1) {
            temp(x, y, configs[0].dBoundaryTemperature);
        }

        if (y == 0) {
            configs[0].dBoundaryTemperature = 1.0;
        }

        if (y == size.y() - 1) {
            configs[0].dBoundaryTemperature = 2.0;
        }
    });

    AddBox(Vector2d(5.5, 0.0), Vector2d(5.0, 0.1), [&](int x, int y, GasesConfigsMap& configs, const Vector2i& size, const Vector2i& start) {
        auto temp = [&](int x, int y, double& temperature) {
            temperature = ((double) y) / size.y() * (2.0 - 1.0) + 1.0;
        };

        if (x == 0) {
            configs[0].eBoundaryType = sep::BT_GASE;
            configs[0].dBoundaryPressure = 1.0;
            temp(x, y, configs[0].dBoundaryTemperature);
        }

        if (x == size.x() - 1) {
            configs[0].eBoundaryType = sep::BT_GASE;
            configs[0].dBoundaryPressure = 1.0;
            temp(x, y, configs[0].dBoundaryTemperature);
        }

        if (y == 0) {
            configs[0].dBoundaryTemperature = 1.0;
        }

        if (y == size.y() - 1) {
            configs[0].dBoundaryTemperature = 2.0;
        }
    });
}

void GridConstructor::PressureBoundaryConditionTestBigArea() {
    /*
    *	Pressure boundary conditions large area
    */

    Config::getInstance()->setCellSize(Vector2d(0.5, 0.01)); // mm

    PushTemperature(1.0);
    PushPressure(1.0);

    AddBox(Vector2d(0.0, 0.0), Vector2d(5.0, 0.1), [&](int x, int y, GasesConfigsMap& configs, const Vector2i& size, const Vector2i& start) {
        auto temp = [&](int x, int y, double& temperature) {
            temperature = ((double) y) / size.y() * (2.0 - 1.0) + 1.0;
        };

        if (x == 0) {
            temp(x, y, configs[0].dBoundaryTemperature);
        }

        if (x == size.x() - 1) {
            temp(x, y, configs[0].dBoundaryTemperature);
        }

        if (y == 0) {
            configs[0].dBoundaryTemperature = 1.0;
        }

        if (y == size.y() - 1) {
            configs[0].dBoundaryTemperature = 2.0;
        }
    });

    AddBox(Vector2d(5.5, 0.0), Vector2d(5.0, 0.1), [&](int x, int y, GasesConfigsMap& configs, const Vector2i& size, const Vector2i& start) {
        auto temp = [&](int x, int y, double& temperature) {
            temperature = ((double) y) / size.y() * (2.0 - 1.0) + 1.0;
        };

        if (x == 0) {
            configs[0].eBoundaryType = sep::BT_GASE;
            configs[0].dBoundaryPressure = 1.0;
            temp(x, y, configs[0].dBoundaryTemperature);
        }

        if (x == size.x() - 1) {
            configs[0].eBoundaryType = sep::BT_GASE;
            configs[0].dBoundaryPressure = 1.0;
            temp(x, y, configs[0].dBoundaryTemperature);
        }

        if (y == 0) {
            configs[0].dBoundaryTemperature = 1.0;
        }

        if (y == size.y() - 1) {
            configs[0].dBoundaryTemperature = 2.0;
        }
    });
}
