#include "grid_constructor.h"
#include "config.h"

// This method being invoked while grid configuration
// Feel free to edit this method
void GridConstructor::ConfigureGridGeometry() {
//    // test 1 =======================================================================
//    Config::vCellSize = Vector2d(1.0, 1.0);
//    PushTemperature(1.5);
//    PushPressure(1.0);
//    SetBox(Vector2d(0, 0), Vector2d(25, 25), [] (int x, int y, CellConfig* config, GridBox* box) {
//      config->wall_T = (x == 0 || x == 24) ? 2.0 : 1.0;
//    });
//    // ==============================================================================


    // test 2 =======================================================================
    Config::vCellSize = Vector2d(1.0, 1.0);
    PushTemperature(1.0);
    PushPressure(1.0);
    SetBox(Vector2d(1, 1), Vector2d(60, 20), [](int x, int y, GasesConfigsMap& configs,
            const Vector2i& size, const Vector2i& start) {
      double dT1 = 1.0;
      double dT2 = 0.5;


      configs[0].pressure = 1.0;
      configs[1].pressure = 0.0;
      if (x == 0)
      {
        configs[0].boundary_cond = sep::BT_PRESSURE;
        configs[0].boundary_pressure = 1.0;
        configs[0].boundary_T = dT1 - (dT1 - dT2) * y / size.y();

        configs[1].boundary_cond = sep::BT_PRESSURE;
        configs[1].boundary_pressure = 0.0;
        configs[1].boundary_T = 1.0;
      }
      if (x == size.x() - 1)
      {
        configs[0].boundary_cond = sep::BT_PRESSURE;
        configs[0].boundary_pressure = 1.0;
        configs[0].boundary_T = dT1 - (dT1 - dT2) * y / size.y();

        configs[1].boundary_cond = sep::BT_STREAM;
        configs[1].boundary_stream = Vector3d(0.02, 0.0, 0.0);
        configs[1].boundary_T = 1.0;
      }
      if (y == 0)
      {
        if (x == int(size.x() * 1.0 / 3.0) || x == int(size.x() * 2.0 / 3.0))
        {
          configs[0].boundary_cond = sep::BT_PRESSURE;
          configs[0].boundary_pressure = 1.0;
          configs[0].boundary_T = dT1;

          configs[1].boundary_cond = sep::BT_STREAM;
          configs[1].boundary_stream = Vector3d(0.0, 0.01, 0.0);
          configs[1].boundary_T = dT1;
        }
        else {
          configs[0].boundary_cond = sep::BT_DIFFUSE;
          configs[0].boundary_T = dT1;

          configs[1].boundary_cond = sep::BT_DIFFUSE;
          configs[1].boundary_T = dT1;
        }
      }
      if (y == size.y() - 1)
      {
        configs[0].boundary_cond = sep::BT_DIFFUSE;
        configs[0].boundary_T = dT2;
      }
      /*
    PushTemperature(0.8);
    PushPressure(1.0);
    SetBox(Vector2d(1, 1), Vector2d(15, 15), [] (int x, int y, GasesConfigsMap& configs, const Vector2i& size) {
        if (x == 0) {
            configs[0].boundary_T = 1.0;
            configs[1].boundary_T = 0.5;
        }
        if (x == size.x() - 1) {
          configs[0].boundary_cond = sep::BT_STREAM;
          configs[0].boundary_stream = Vector3d(-0.01, 0.0, 0.0);
          configs[0].boundary_T = 1.0;
        }
        */
    });
    /*
    SetBox(Vector2d(10, 10), Vector2d(15, 15), [] (int x, int y, CellConfig* config, GridBox* box) {
=======
    SetBox(Vector2d(10, 10), Vector2d(15, 15), [] (int x, int y, GasesConfigsMap& configs, struct GridBox* box) {
>>>>>>> origin/master
//        config->wall_T = (x == 0 || x == 24) ? 2.0 : 1.0;
         if (x == box->size.x() - 1) {
            configs[0].boundary_cond = sep::BT_STREAM;
            configs[0].boundary_stream = Vector3d(-1.0, 0.0, 0.0);
        }
    });
    */
    // ==============================================================================

//    // test 3 =======================================================================
//    //Config::vCellSize = Vector2d(8.0, 0.25);
//    Config::vCellSize = Vector2d(2.0, 0.25);
//    double T1 = 325.0 + 273.0;
//    double T2 = 60.0 + 273.0;
//    T1 /= 350.0;    // TODO: precise normalization
//    T2 /= 350.0;
//    PushTemperature(T1);
//    // box 1
//    PushPressure(1.1);
//    SetBox(Vector2d(-20, 0), Vector2d(150, 4), [] (int x, int y, CellConfig* config, GridBox* box) {
//        if (x == 0) {
//            config->boundary_cond = sep::BT_PRESSURE;
//            config->boundary_pressure = 1.3;
//        }
//    });
//    // box 2
//    PushPressure(1.05);
//    SetBox(Vector2d(100, 0), Vector2d(30, 12.5), [] (int x, int y, CellConfig* config, GridBox* box) {});
//    // box 3
//    PushPressure(0.5);
//    SetBox(Vector2d(30, 11.25), Vector2d(100, 1.25), [] (int x, int y, CellConfig* config, GridBox* box) {
//        if (x == 0) {
//            config->boundary_cond = sep::BT_STREAM;    // should be gas <-> fluid bound
//            config->boundary_stream = Vector3d(1.0, 0.0, 0.0);
//        }
//    });
//    // ==============================================================================


    // add here your configuration
}