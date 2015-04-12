#include "grid_constructor.h"
#include "config.h"

// This method being invoked while grid configuration
// Feel free to edit this method
void GridConstructor::ConfigureStandartGrid() {

    // Main configuration part
    // normalization base
    Config::T_normalize = 1800.0;  // K // Maximum temperature in system
    Config::n_normalize = 1.207e22;  // 1 / m^3 // P/(kT) P = 150Pa T = 900K
    Config::P_normalize = Config::n_normalize * sep::k * Config::T_normalize;
    Config::m_normalize = 133 * 1.66e-27;   // kg
    Config::e_cut_normalize = std::sqrt(sep::k * Config::T_normalize / Config::m_normalize); // m / s
    Config::l_normalize = 0.1e-4; // m
    Config::tau_normalize = Config::l_normalize / Config::e_cut_normalize;  // s

    

    PushTemperature(1.0);
    PushPressure(1.0);

    //Vector2d vPhysSize = Vector2d(520.0, 0.4);
    //Vector2i vNumSize = Vector2i(88, 30);
    //Config::vCellSize = Vector2d(vPhysSize.x()/vNumSize.x(), vPhysSize.y()/vNumSize.y());

    //vPhysSize

    SetBox(Vector2d(0.0, 0.0), Vector2d(100.0, 30.0), [](int x, int y, GasesConfigsMap& configs,
            const Vector2i& size, const Vector2i& start) {
      
      double dT1 = 1800.0 / Config::T_normalize;
      double dT2 = 900.0 / Config::T_normalize;
      
      int iNumHoles = 9;
      double dSummaryStream = 0.000009; // 5.22e18


      configs[0].pressure = 1.0;
      configs[0].T = dT1 - (dT1 - dT2) * y / size.y();

      configs[1].pressure = 0.0;
      
      if (x == 0)
      {
        configs[0].boundary_cond = sep::BT_DIFFUSE;
        configs[0].boundary_pressure = 1.0;
        configs[0].boundary_T = dT1 - (dT1 - dT2) * y / size.y();

        configs[1].boundary_cond = sep::BT_PRESSURE;
        configs[1].boundary_pressure = 0.0;
        configs[1].boundary_T = 1.0;
      }
      if (x == size.x() - 1)
      {
        configs[0].boundary_cond = sep::BT_DIFFUSE;
        configs[0].boundary_pressure = 1.0;
        configs[0].boundary_T = dT1 - (dT1 - dT2) * y / size.y();

        configs[1].boundary_cond = sep::BT_STREAM;
        configs[1].boundary_stream = Vector3d(dSummaryStream, 0.0, 0.0);
        configs[1].boundary_T = 1.0;
      }
      if (y == 0)
      {
        bool isAHole = false;
        for (int i = 0; i < iNumHoles; i++)
        {
          isAHole = isAHole || x == int(size.x() * double(i + 1) / (iNumHoles + 1));
        }
        if (isAHole)
        {
          configs[0].boundary_cond = sep::BT_DIFFUSE;
          configs[0].boundary_pressure = 1.0;
          configs[0].boundary_T = dT1;

          configs[1].boundary_cond = sep::BT_STREAM;
          configs[1].boundary_stream = Vector3d(0.0, dSummaryStream / iNumHoles, 0.0);
          configs[1].boundary_T = dT1;
        }
        else {
          configs[0].boundary_cond = sep::BT_DIFFUSE;
          configs[0].boundary_T = dT1;

          configs[1].boundary_cond = sep::BT_DIFFUSE;
          configs[1].boundary_T = dT1;
        }
      }
      if (y == size.y() - 1) {
          configs[0].boundary_cond = sep::BT_DIFFUSE;
          configs[0].boundary_T = dT2;

          configs[1].boundary_cond = sep::BT_DIFFUSE;
          configs[1].boundary_T = dT2;
      }
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