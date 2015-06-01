#include "grid_constructor.h"
#include "config.h"
#include "parameters/beta_chain.h"

#define M_PI           3.14159265358979323846  /* pi */

// This method being invoked while grid configuration
// Feel free to edit this method
void GridConstructor::ConfigureStandartGrid() {

    // Main configuration part
    // normalization base
    Config::T_normalize = 1500.0;  // K // Maximum temperature in system
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
		Vector2d vPhysSize = Vector2d(520.0, 0.4);
    Vector2i vNumSize = Vector2i(10, 10);
    Config::vCellSize = Vector2d(vPhysSize.x() / vNumSize.x(), vPhysSize.y() / vNumSize.y());

		SetBox(Vector2d(0.0, 0.0), vPhysSize, [](int x, int y, GasesConfigsMap& configs,
			const Vector2i& size, const Vector2i& start) 
		{
			// Task parameters
			double T1 = 1500.0 / Config::T_normalize;
			double T2 = 900.0 / Config::T_normalize;

			double dPCsLeft = 450.0 / Config::P_normalize;
			double dPCsRight = 150.0 / Config::P_normalize;

			double dPKr = 7e-7 / Config::P_normalize;
			double dPXe = 1.2e-6 / Config::P_normalize;

			double dSpeedKr = 2.1e-9 / (dPKr * M_PI * (0.008 * 0.008) / 4) / Config::e_cut_normalize; // Q/(P*S) S = pi * D^2 / 4
			double dSpeedXe = 3.6e-9 / (dPXe * M_PI * (0.008 * 0.008) / 4) / Config::e_cut_normalize; // Q/(P*S) S = pi * D^2 / 4

			int iNumHoles = 9;

			configs[0].pressure = dPCsLeft + (dPCsLeft - dPCsRight) * x / (size.x() - 1);
			configs[0].T = T1 - (T1 - T2) * y / (size.y() - 1);

			for (int i = 1; i < Config::iGasesNumber; i++) {
				configs[i].pressure = 0.0;
				configs[i].T = T1 - (T1 - T2) * y / (size.y() - 1);
			}

			if (x == 0) { // left border
				configs[0].boundary_cond = sep::BT_DIFFUSE;
				configs[0].boundary_pressure = dPCsLeft;
				configs[0].boundary_T = T1 - (T1 - T2) * y / (size.y() - 1);

				for (int i = 1; i < Config::iGasesNumber; i++) {
					configs[i].boundary_cond = sep::BT_DIFFUSE;
					configs[i].boundary_T = std::sqrt(T1 * T2);
				}
			}
			if (x == size.x() - 1) { // right border
				configs[0].boundary_cond = sep::BT_DIFFUSE;
				configs[0].boundary_pressure = dPCsRight;
				configs[0].boundary_T = T1 - (T1 - T2) * y / (size.y() - 1);

				for (int i = 1; i < Config::iGasesNumber; i++) {
					configs[i].boundary_cond = sep::BT_GASE;
					configs[i].boundary_pressure = 0.0;
					configs[i].boundary_stream = Vector3d();
					configs[i].boundary_T = std::sqrt(T1 * T2);
				}
			}
			if (y == 0) { // bottom border
				if (x % ((size.x() - 1) / (iNumHoles + 1)) == 0) { // a hole
					configs[0].boundary_cond = sep::BT_DIFFUSE;
					configs[0].boundary_T = T1;

					// Kr
					configs[1].boundary_cond = sep::BT_GASE;
					configs[1].boundary_pressure = dPKr;
					configs[1].boundary_stream = Vector3d(0.0, dSpeedKr * dPKr / T1, 0.0);
					configs[1].boundary_T = T1;

					// Xe
					configs[2].boundary_cond = sep::BT_GASE;
					configs[2].boundary_pressure = dPXe;
					configs[2].boundary_stream = Vector3d(0.0, dSpeedXe* dPKr / T1, 0.0);
					configs[2].boundary_T = T1;

					for (int i = 3; i < Config::iGasesNumber; i++) {
						configs[i].boundary_cond = sep::BT_DIFFUSE;
						configs[i].boundary_T = T1;
					}
				}
				else {
					configs[0].boundary_cond = sep::BT_DIFFUSE;
					configs[0].boundary_T = T1;

					for (int i = 1; i < Config::iGasesNumber; i++) {
						configs[i].boundary_cond = sep::BT_DIFFUSE;
						configs[i].boundary_T = T1;
					}
				}
			}
			if (y == size.y() - 1) { // top border
				configs[0].boundary_cond = sep::BT_DIFFUSE;
				configs[0].boundary_T = T2;
			}
			
		});
}