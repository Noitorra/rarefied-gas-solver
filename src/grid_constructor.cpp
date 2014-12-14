#include "grid_constructor.h"
#include "config.h"

// This method being invoked while grid configuration
// Feel free to edit this method
void GridConstructor::ConfigureGridGeometry() {
//    // test 1 =======================================================================
//    Config::vCellSize = Vector2d(1.0, 1.0);
//    PushTemperature(1.5);
//    PushConcentration(1.0);
//    SetBox(Vector2d(0, 0), Vector2d(25, 25), [] (int x, int y, CellConfig* config) {
//      config->wall_T = (x == 0 || x == 24) ? 2.0 : 1.0;
//    });
//    // ==============================================================================


    // test 2 =======================================================================
    Config::vCellSize = Vector2d(1.0, 1.0);
    PushTemperature(1.5);
    PushConcentration(1.0);
    SetBox(Vector2d(1, 1), Vector2d(15, 15), [] (int x, int y, CellConfig* config) {
      config->wall_T = (x == 0) ? 2.0 : 1.0;
    });
    SetBox(Vector2d(10, 10), Vector2d(15, 15), [] (int x, int y, CellConfig* config) {
//        config->wall_T = (x == 0 || x == 24) ? 2.0 : 1.0;
    });
    // ==============================================================================

//    //Config::vCellSize = Vector2d(8.0, 0.25);
//    Config::vCellSize = Vector2d(2.0, 0.25);
//    double T1 = 325.0 + 273.0;
//    double T2 = 60.0 + 273.0;
//    T1 /= 300.0;    // TODO: precise normalization
//    T2 /= 300.0;
//    PushTemperature(T1);
//    // box 1
//    PushConcentration(1.1);
//    SetBox(Vector2d(-20, 0), Vector2d(150, 4), [] (int x, int y, CellConfig* config) {
//        if (x == 0)
//            config->boundary_cond = sep::BT_STREAM_PRESSURE;
//    });
//    // box 2
//    PushConcentration(1.05);
//    SetBox(Vector2d(100, 0), Vector2d(30, 12.5), [] (int x, int y, CellConfig* config) {});
//    // box 3
//    PushConcentration(0.5);
//    SetBox(Vector2d(30, 11.25), Vector2d(100, 1.25), [] (int x, int y, CellConfig* config) {
//        if (x == 0)
//            config->boundary_cond = sep::BT_DIFFUSE;    // gas <-> fluid bound
//    });
}