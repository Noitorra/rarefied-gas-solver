#include "grid_constructor.h"
#include "config.h"
#include "parameters/beta_chain.h"

// This method being invoked while grid configuration
// Feel free to edit this method
void GridConstructor::ConfigureStandartGrid() {

    // Main configuration part
    // normalization base
    Config::T_normalize = 1800.0;  // K // Maximum temperature in system
		Config::P_normalize = 150.0;
		Config::n_normalize = Config::P_normalize / sep::k / Config::T_normalize;
    Config::m_normalize = 133 * 1.66e-27;   // kg
    Config::e_cut_normalize = std::sqrt(sep::k * Config::T_normalize / Config::m_normalize); // m / s
    Config::l_normalize = 1e-4; // m для Cs !!!
    Config::tau_normalize = Config::l_normalize / Config::e_cut_normalize;  // s

		// Some beta chain hack. Makes lambda without data type.
		for (auto& item : Config::vBetaChains) {
			item->dLambda1 *= Config::tau_normalize;
			item->dLambda2 *= Config::tau_normalize;
		}
    

    PushTemperature(1.0);
    PushPressure(1.0);

    //Vector2d vPhysSize = Vector2d(520.0, 0.4);
		Vector2d vPhysSize = Vector2d(2.0, 0.4);
    Vector2i vNumSize = Vector2i(100, 20);
    Config::vCellSize = Vector2d(vPhysSize.x() / vNumSize.x(), vPhysSize.y() / vNumSize.y());

		SetBox(Vector2d(0.0, 0.0), vPhysSize, [](int x, int y, GasesConfigsMap& configs,
			const Vector2i& size, const Vector2i& start) 
		{
			// Task parameters
			double T1 = 1800.0 / Config::T_normalize;
			double T2 = 900.0 / Config::T_normalize;

			configs[0].pressure = 1.0;
			//configs[0].T = T1 - (T1 - T2) * y / size.y();
			configs[0].T = std::sqrt(T1 * T2);

			if (x == 0) { // left border
				configs[0].boundary_cond = sep::BT_PRESSURE;
				configs[0].boundary_pressure = 1.0;
				//configs[0].boundary_stream = Vector3d(1.0, 0.0, 0.0);
				configs[0].boundary_T = std::sqrt(T1 * T2);
				//configs[0].boundary_T = T1 - (T1 - T2) * y / size.y();
			}
			if (x == size.x() - 1) { // right border
				configs[0].boundary_cond = sep::BT_PRESSURE;
				configs[0].boundary_pressure = 1.0;
				configs[0].boundary_T = std::sqrt(T1 * T2);
				//configs[0].boundary_T = T1 - (T1 - T2) * y / size.y();
			}
			if (y == 0) { // bottom border
				configs[0].boundary_cond = sep::BT_DIFFUSE;
				configs[0].boundary_T = T1;
			}
			if (y == size.y() - 1) { // top border
				configs[0].boundary_cond = sep::BT_DIFFUSE;
				configs[0].boundary_T = T2;
			}
			
		});
}