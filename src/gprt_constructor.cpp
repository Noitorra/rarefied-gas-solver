#include "grid_constructor.h"
#include "config.h"

// TODO: solve this problem of lambdas
double T1, T2, P_sat;

//void GridConstructor::ConfigureGPRT() {
//    //    Config::vCellSize = Vector2d(8.0, 0.25);
////    Config::vCellSize = Vector2d(3.0, 0.4);
//    Config::vCellSize = Vector2d(8.0, 0.4);
//    T1 = 325.0 + 273.0;
//    T2 = 60.0 + 273.0;
//    Vector2d walls(Config::vCellSize);
//    Vector2d sp_delta(Config::vCellSize);
////    sp_delta /= 2.0;
//    walls *= 2.0;
//    T1 /= 600.0;    // TODO: precise normalization
//    T2 /= 600.0;
//    PushTemperature(T1);
//    P_sat = 1.0; // TODO: set saturated pressure
//    // debug
//    T1 = 1.0;
//    T2 = 0.5;
//
//    auto global_temp = [&] (Vector2i p_abs, double& temperature) {
//        Vector2d p(p_abs.x() * Config::vCellSize.x(), p_abs.y() * Config::vCellSize.y());
////        std::cout << "T1 = " << T1 << " " << T2 << std::endl;
//        const double sp_delta_wall = 30.0;
//        const double temp_start = 100.0 + sp_delta_wall;
//        const double temp_finish = 350.0;
//        if (p.x() < temp_start) {
//            temperature = T1;
//            return;
//        }
//        if (p.x() > temp_finish) {
//            temperature = T2;
//            return;
//        }
//        temperature = (p.x() - temp_start) / (temp_finish - temp_start) * (T2 - T1) + T1;
//        std::cout << "T = " << temperature << std::endl;
//
//    };
//    auto global_pressure = [=] (Vector2i p_abs, double& pressure) {
//        Vector2d p(p_abs.x() * Config::vCellSize.x(), p_abs.y() * Config::vCellSize.y());
//        pressure = (1.0 - (p.x() - 30.0) / 400.0) * P_sat;
//        std::cout << "P = " << pressure << std::endl;
//    };
//
//    // box 1
////    PushPressure(1.0);
//    PushPressure(P_sat);
//    SetBox(Vector2d(-20.0, 0.0), Vector2d(150.0 - sp_delta.x(), 4.0),
//            [] (int x, int y, GasesConfigsMap& configs, const Vector2i& size, const Vector2i& start) {
//
//                if (x == 0) {
//                    configs[0].boundary_cond = sep::BT_PRESSURE;
//                    configs[0].boundary_pressure = 1.3;
//                }
//                if (y == 1) {
//                    configs[0].mirror_type = sep::MT_ENABLED;
//                    configs[0].mirror_axis = sep::X;
//                }
//            });
//
//    // box 2
////    PushPressure(0.95);
//    SetBox(Vector2d(100.0, 0.0), Vector2d(30.0, 12.5),
//            [] (int x, int y, GasesConfigsMap& configs, const Vector2i& size, const Vector2i& start) {});
//
//    // box 3
////    PushPressure(0.9);
//    SetBox(Vector2d(30.0, 11.25), Vector2d(100.0, 1.25),
//            [] (int x, int y, GasesConfigsMap& configs, const Vector2i& size, const Vector2i& start) {
//
//            if (x == 0) {
//                configs[0].boundary_cond = sep::BT_STREAM;    // should be gas <-> fluid bound
//                configs[0].boundary_stream = Vector3d(0.01, 0.0, 0.0);
//            }
//    });
//
//    // box 4
//    PushPressure(0.85);
//    SetBox(Vector2d(30.0, 15.5), Vector2d(100.0 + walls.x(), 2.5),
//            [&] (int x, int y, GasesConfigsMap& configs, const Vector2i& size, const Vector2i& start) {
//
//            Vector2i abs_p(x + start.x(), y + start.y());
//            global_temp(abs_p, configs[0].T);
//            global_temp(abs_p, configs[0].boundary_T);
//            global_pressure(abs_p, configs[0].pressure);
//    });
//
//    // box 5
//    PushPressure(0.8);
//    SetBox(Vector2d(130.0 + walls.x() - sp_delta.x(), 0.0), Vector2d(200.0 - walls.x(), 18.0 - sp_delta.y()),
//            [&] (int x, int y, GasesConfigsMap& configs, const Vector2i& size, const Vector2i& start) {
//
//            Vector2i abs_p(x + start.x(), y + start.y());
//            global_temp(abs_p, configs[0].T);
//            global_temp(abs_p, configs[0].boundary_T);
//            global_pressure(abs_p, configs[0].pressure);
//    });
//
//    // box 6
//    PushPressure(0.75);
//    SetBox(Vector2d(330.0 - sp_delta.x(), 0.0), Vector2d(100.0, 8.0),
//            [&] (int x, int y, GasesConfigsMap& configs, const Vector2i& size, const Vector2i& start) {
//
//        Vector2i abs_p(x + start.x(), y + start.y());
//        global_temp(abs_p, configs[0].T);
//        global_temp(abs_p, configs[0].boundary_T);
//        global_pressure(abs_p, configs[0].pressure);
//
//        if (x == size.x()) {
//            configs[0].boundary_cond = sep::BT_PRESSURE;
//            configs[0].boundary_pressure = 0.0;
//            configs[0].boundary_T = T2;
//        }
//    });
//
//    // box 7
//    PushPressure(0.7);
//    SetBox(Vector2d(130.0 - sp_delta.x(), 0.0), Vector2d(walls.x(), Config::vCellSize.y() * 4),
//            [] (int x, int y, GasesConfigsMap& configs, const Vector2i& size, const Vector2i& start) {});
//}

void GridConstructor::ConfigureGPRT() {

    PushTemperature(0.5);
    SetBox(Vector2d(0.0, 0.0), Vector2d(10.0, 10.0),
        [] (int x, int y, GasesConfigsMap& configs, const Vector2i& size, const Vector2i& start) {

            if (y == size.y() - 1) {
                configs[0].boundary_T = 1.0;
            }
            if (x == 0) {
                configs[0].mirror_type = sep::MT_BEGIN;
                configs[0].mirror_axis = sep::X;
            }
        });
}

