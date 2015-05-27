#include "grid_constructor.h"
#include "config.h"
#include "parameters/beta_chain.h"

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

		// Some beta chain hack. Makes lambda without data type.
		for (auto& item : Config::vBetaChains) {
			item->dLambda1 *= Config::tau_normalize;
			item->dLambda2 *= Config::tau_normalize;
		}
    

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

			for (int i = 1; i < Config::iGasesNumber; i++) {
				configs[i].pressure = 0.0; // no gas
			}
      
      if (x == 0) // left border
      {
        configs[0].boundary_cond = sep::BT_PRESSURE;
				configs[0].boundary_pressure = 1.0;
        configs[0].boundary_T = dT1 - (dT1 - dT2) * y / size.y();

        //configs[1].boundary_cond = sep::BT_STREAM;
				//configs[1].boundary_stream = Vector3d(0.0, 0.0, 0.0);
				//configs[1].boundary_T = dT1 - (dT1 - dT2) * y / size.y();
      }
      if (x == size.x() - 1) // right border
      {
				configs[0].boundary_cond = sep::BT_PRESSURE;
				configs[0].boundary_pressure = 1.0;
        configs[0].boundary_T = dT1 - (dT1 - dT2) * y / size.y();

        //configs[1].boundary_cond = sep::BT_STREAM;
        //configs[1].boundary_stream = Vector3d(dSummaryStream, 0.0, 0.0);
        //configs[1].boundary_T = 1.0;
      }
      if (y == 0) // bottom border
      {
        bool isAHole = false;
        for (int i = 0; i < iNumHoles; i++)
        {
          isAHole = isAHole || x == int(size.x() * double(i + 1) / (iNumHoles + 1));
        }
        if (isAHole) // a hole
        {
          configs[0].boundary_cond = sep::BT_DIFFUSE; // maybe stream with 0, which is exactly the same
          configs[0].boundary_pressure = 1.0;
          configs[0].boundary_T = dT1;

          //configs[1].boundary_cond = sep::BT_STREAM;
          //configs[1].boundary_stream = Vector3d(0.0, dSummaryStream / iNumHoles, 0.0);
          //configs[1].boundary_T = dT1;
        }
        else {
          configs[0].boundary_cond = sep::BT_DIFFUSE;
          configs[0].boundary_T = dT1;

          //configs[1].boundary_cond = sep::BT_DIFFUSE;
          //configs[1].boundary_T = dT1;
        }
      }
      if (y == size.y() - 1) { // top border
          configs[0].boundary_cond = sep::BT_DIFFUSE;
          configs[0].boundary_T = dT2;

          //configs[1].boundary_cond = sep::BT_DIFFUSE;
          //configs[1].boundary_T = dT2;
      }
    });
}