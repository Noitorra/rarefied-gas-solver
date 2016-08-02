#include "grid_constructor.h"
#include "config.h"
#include "parameters/beta_chain.h"


void global_temp(Vector2d p, double& temperature, double box_6_start_x, double T1, double T2) {
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

}

void global_pressure_Cs(Vector2d p, double& pressure, double P_sat_T1) {
	pressure = (1.0 - (p.x() - 30.0) / 400.0) * P_sat_T1;
	//std::cout << "P = " << pressure << std::endl;
}

void global_pressure_Xe(Vector2d p, double& pressure, double P_sat_Xe) {
	pressure = (1.0 - (p.x() - 30.0) / 400.0) * P_sat_Xe;
}

void global_pressure_Kr(Vector2d p, double& pressure, double P_sat_Kr) {
	pressure = (1.0 - (p.x() - 30.0) / 400.0) * P_sat_Kr;
}

double gradient(double i, double max_i, double from, double to) {
	return i / max_i * (to - from) + from;
}

double T1;
double T2;

double P_sat_T1;
double P_sat_T2;
double Q_Xe_in;
double P_Xe_in;
double P_sat_Xe;

double Q_Kr_in;
double P_Kr_in;
double P_sat_Kr;

double box_6_start_x;

void GridConstructor::ConfigureGPRT() {

	Config::vCellSize = Vector2d(12.0, 0.4); // mm
	T1 = 325.0 + 273.0;
	T2 = 60.0 + 273.0;
	Vector2d walls(Config::vCellSize);
	Vector2d sp_delta(Config::vCellSize);
	walls *= 2.0;

	// normalization base
	Config::T_normalize = 600.0; // K
	Config::n_normalize = 1.81e22; // 1 / m^3
	Config::P_normalize = Config::n_normalize * sep::k * Config::T_normalize;
	Config::m_normalize = 133 * 1.66e-27; // kg
	Config::e_cut_normalize = sqrt(sep::k * Config::T_normalize / Config::m_normalize); // m / s
	Config::l_normalize = 0.5 * 6e-4; // m +
	//Config::l_normalize = 0.2 * 6e-4; // m -
	Config::tau_normalize = Config::l_normalize / Config::e_cut_normalize; // s

	T1 /= Config::T_normalize;
	T2 /= Config::T_normalize;
	PushTemperature(T1);
	P_sat_T1 = 1.0; // 150Pa, T = T0, n = n0
	P_sat_T2 = 2.7e-4 / Config::P_normalize; // 2.7 x 10^-4 Pa ��� 320 K
	Q_Xe_in = 8.6e15 / (Config::n_normalize * Config::e_cut_normalize); // 8.6 x 10^15 1/(m^2 * s)
	P_Xe_in = 1.2e-6 / Config::P_normalize; // 1.2 X 10^-6 Pa
	P_sat_Xe = P_Xe_in * 0.5;

	// Kr
	Q_Kr_in = 5.0e15 / (Config::n_normalize * Config::e_cut_normalize); // 5.0 x 10^15 1/(m^2 * s)
	P_Kr_in = 7e-7 / Config::P_normalize; // 7 X 10^-7 Pa
	P_sat_Kr = P_Kr_in * 0.5;

	box_6_start_x = 330.0 - 3 * sp_delta.x();

	// Some beta chain hack. Makes lambda without data type.
	for (auto& item : Config::vBetaChains) {
		item->dLambda1 *= Config::tau_normalize;
		item->dLambda2 *= Config::tau_normalize;
	}

	// boxes ========================================================================
	// box 1
	//    PushPressure(1.0);
	PushPressure(P_sat_T1);

	struct Box1: GridBox {

		virtual void config(int x, int y, GasesConfigsMap& configs, const Vector2i& size, const Vector2i& start) {
			configs[1].pressure = gradient(x, size.x() - 1, P_Xe_in, P_Xe_in + (P_sat_Xe - P_Xe_in) * 0.3);
			configs[2].pressure = gradient(x, size.x() - 1, P_Kr_in, P_Kr_in + (P_sat_Kr - P_Kr_in) * 0.3);

			if (x == 0) {
				configs[0].boundary_cond = sep::BT_GASE;
				configs[0].boundary_stream = Vector3d();
				configs[0].boundary_pressure = P_sat_T1;
				configs[0].boundary_T = T1;

				configs[1].boundary_cond = sep::BT_GASE;
				configs[1].boundary_stream = Vector3d(Q_Xe_in, 0.0, 0.0);
				configs[1].boundary_pressure = P_Xe_in;
				configs[1].boundary_T = T1;

				configs[2].boundary_cond = sep::BT_GASE;
				configs[2].boundary_stream = Vector3d(Q_Kr_in, 0.0, 0.0);
				configs[2].boundary_pressure = P_Kr_in;
				configs[2].boundary_T = T1;
			}

			//                if (y == 0) {
			//                    for (int gas = 0; gas < Config::iGasesNumber; gas++) {
			//                        configs[gas].boundary_cond = sep::BT_MIRROR;
			//                    }
			//                }
		}
	};

	AddBox(Vector2d(-20.0, 0.0), Vector2d(150.0 - sp_delta.x(), 4.0), new Box1());

	// box 2
	//    PushPressure(0.95);
	struct Box2: GridBox {

		virtual void config(int x, int y, GasesConfigsMap& configs, const Vector2i& size, const Vector2i& start) {
			configs[1].pressure = gradient(y, size.y() - 1, P_Xe_in + (P_sat_Xe - P_Xe_in) * 0.3, P_Xe_in + (P_sat_Xe - P_Xe_in) * 0.6);
			configs[2].pressure = gradient(y, size.y() - 1, P_Kr_in + (P_sat_Kr - P_Kr_in) * 0.3, P_Kr_in + (P_sat_Kr - P_Kr_in) * 0.6);

			//                if (y == 0) {
			//                    for (int gas = 0; gas < Config::iGasesNumber; gas++) {
			//                        configs[gas].boundary_cond = sep::BT_MIRROR;
			//                    }
			//                }
		}
	};

	AddBox(Vector2d(100.0, 0.0), Vector2d(30.0, 12.5), new Box2());

	// box 3
	//    PushPressure(0.9);
	struct Box3: GridBox {

		virtual void config(int x, int y, GasesConfigsMap& configs, const Vector2i& size, const Vector2i& start) {
			configs[1].pressure = gradient(x, size.x() - 1, P_sat_Xe, P_Xe_in + (P_sat_Xe - P_Xe_in) * 0.6);
			configs[2].pressure = gradient(x, size.x() - 1, P_sat_Kr, P_Kr_in + (P_sat_Kr - P_Kr_in) * 0.6);

			if (x == 0) {
				configs[0].boundary_cond = sep::BT_GASE; // should be gas <-> fluid bound
				configs[0].boundary_pressure = P_sat_T1;
				configs[0].boundary_T = T1;

				configs[1].boundary_cond = sep::BT_GASE; // should be adsorption
				configs[1].boundary_pressure = P_sat_Xe;
				configs[1].boundary_T = T1;

				configs[2].boundary_cond = sep::BT_GASE; // should be adsorption
				configs[2].boundary_pressure = P_sat_Kr;
				configs[2].boundary_T = T1;
			}
		}
	};

	AddBox(Vector2d(30.0, 11.25), Vector2d(100.0, 1.25), new Box3());

	// box 4
	//    PushPressure(0.85);
	struct Box4: GridBox {

		virtual void config(int x, int y, GasesConfigsMap& configs, const Vector2i& size, const Vector2i& start) {
			Vector2i p_abs(x + start.x(), y + start.y());
			Vector2d p(p_abs.x() * Config::vCellSize.x(), p_abs.y() * Config::vCellSize.y());

			global_temp(p, configs[0].T, box_6_start_x, T1, T2);
			global_temp(p, configs[0].boundary_T, box_6_start_x, T1, T2);
			global_pressure_Cs(p, configs[0].pressure, P_sat_T1);

			global_temp(p, configs[1].T, box_6_start_x, T1, T2);
			global_temp(p, configs[1].boundary_T, box_6_start_x, T1, T2);
			global_pressure_Xe(p, configs[1].pressure, P_sat_Xe);

			global_temp(p, configs[2].T, box_6_start_x, T1, T2);
			global_temp(p, configs[2].boundary_T, box_6_start_x, T1, T2);
			global_pressure_Kr(p, configs[2].pressure, P_sat_Kr);

			if (x == 0) {
				configs[0].boundary_cond = sep::BT_GASE; // should be fluid <-> gas bound
				configs[0].boundary_pressure = P_sat_T1;
				configs[0].boundary_T = T1;

				configs[1].boundary_cond = sep::BT_GASE; // should be adsorption
				configs[1].boundary_pressure = P_sat_Xe;
				configs[1].boundary_T = T1;

				configs[2].boundary_cond = sep::BT_GASE; // should be adsorption
				configs[2].boundary_pressure = P_sat_Kr;
				configs[2].boundary_T = T1;
			}
		}
	};

	AddBox(Vector2d(30.0, 15.5), Vector2d(100.0 + walls.x(), 2.5), new Box4());

	// box 5
	//    PushPressure(0.8);
	struct Box5: GridBox {

		virtual void config(int x, int y, GasesConfigsMap& configs, const Vector2i& size, const Vector2i& start) {
			Vector2i p_abs(x + start.x(), y + start.y());
			Vector2d p(p_abs.x() * Config::vCellSize.x(), p_abs.y() * Config::vCellSize.y());

			global_temp(p, configs[0].T, box_6_start_x, T1, T2);
			global_temp(p, configs[0].boundary_T, box_6_start_x, T1, T2);
			global_pressure_Cs(p, configs[0].pressure, P_sat_T1);

			global_temp(p, configs[1].T, box_6_start_x, T1, T2);
			global_temp(p, configs[1].boundary_T, box_6_start_x, T1, T2);
			global_pressure_Xe(p, configs[1].pressure, P_sat_Xe);

			global_temp(p, configs[2].T, box_6_start_x, T1, T2);
			global_temp(p, configs[2].boundary_T, box_6_start_x, T1, T2);
			global_pressure_Kr(p, configs[2].pressure, P_sat_Kr);

			//            if (y == 0) {
			//                for (int gas = 0; gas < Config::iGasesNumber; gas++) {
			//                    configs[gas].boundary_cond = sep::BT_MIRROR;
			//                }
			//            }
		}
	};

	AddBox(Vector2d(130.0 + walls.x()/* - sp_delta.x()*/, 0.0), Vector2d(200.0 - walls.x(), 18.0 - sp_delta.y()), new Box5());
	// box 6
	//    PushPressure(0.75);
	struct Box6: GridBox {

		virtual void config(int x, int y, GasesConfigsMap& configs, const Vector2i& size, const Vector2i& start) {
			Vector2i p_abs(x + start.x(), y + start.y());
			Vector2d p(p_abs.x() * Config::vCellSize.x(), p_abs.y() * Config::vCellSize.y());

			global_temp(p, configs[0].T, box_6_start_x, T1, T2);
			global_temp(p, configs[0].boundary_T, box_6_start_x, T1, T2);
			global_pressure_Cs(p, configs[0].pressure, P_sat_T1);

			global_temp(p, configs[1].T, box_6_start_x, T1, T2);
			global_temp(p, configs[1].boundary_T, box_6_start_x, T1, T2);
			global_pressure_Xe(p, configs[1].pressure, P_sat_Xe);

			global_temp(p, configs[2].T, box_6_start_x, T1, T2);
			global_temp(p, configs[2].boundary_T, box_6_start_x, T1, T2);
			global_pressure_Kr(p, configs[2].pressure, P_sat_Kr);

			if (x == size.x() - 1) {
				for (int gas = 0; gas < Config::iGasesNumber; gas++) {
					configs[gas].boundary_cond = sep::BT_GASE;
					configs[gas].boundary_pressure = 0.0;
					configs[gas].boundary_T = T2;
				}
			}

			//                if (y == 0) {
			//                    for (int gas = 0; gas < Config::iGasesNumber; gas++) {
			//                        configs[gas].boundary_cond = sep::BT_MIRROR;
			//                    }
			//                }

			// gas <-> fluid
			if (y == size.y() - 1 && p.x() > box_6_start_x + 50.0) {
				configs[0].boundary_cond = sep::BT_GASE;
				configs[0].boundary_pressure = P_sat_T2;
				configs[0].boundary_T = T2;
			}
		}
	};

	AddBox(Vector2d(box_6_start_x, 0.0), Vector2d(100.0, 8.0), new Box6());

	// box 7
	//    PushPressure(0.7);

	struct Box7: GridBox {
		virtual void config(int x, int y, GasesConfigsMap& configs, const Vector2i& size, const Vector2i& start) {
			if (x != 0 && x != size.x() - 1) {
				configs[0].locked_axes = sep::Y;
				configs[1].locked_axes = sep::Y;
				configs[2].locked_axes = sep::Y;
			}

			configs[0].pressure = gradient(x, size.x() - 1, P_sat_T1, P_sat_T1 * 0.65);
			configs[1].pressure = gradient(x, size.x() - 1, P_Xe_in + (P_sat_Xe - P_Xe_in) * 0.3, P_sat_Xe * 0.65);
			configs[2].pressure = gradient(x, size.x() - 1, P_Kr_in + (P_sat_Kr - P_Kr_in) * 0.3, P_sat_Kr * 0.65);

			Vector2i p_abs(x + start.x(), y + start.y());
			Vector2d p(p_abs.x() * Config::vCellSize.x(), p_abs.y() * Config::vCellSize.y());

			global_temp(p, configs[0].T, box_6_start_x, T1, T2);
			global_temp(p, configs[0].boundary_T, box_6_start_x, T1, T2);

			global_temp(p, configs[1].T, box_6_start_x, T1, T2);
			global_temp(p, configs[1].boundary_T, box_6_start_x, T1, T2);

			global_temp(p, configs[2].T, box_6_start_x, T1, T2);
			global_temp(p, configs[2].boundary_T, box_6_start_x, T1, T2);

			//                if (y == 0) {
			//                    for (int gas = 0; gas < Config::iGasesNumber; gas++) {
			//                        configs[gas].boundary_cond = sep::BT_MIRROR;
			//                    }
			//                }
		}
	};

	AddBox(Vector2d(130.0/* - sp_delta.x()*/, 0.0), Vector2d(walls.x(), Config::vCellSize.y()), new Box7());
}

void GridConstructor::ConfigureGPRT2() {

	PushTemperature(0.5);

	struct GPRT2Box: GridBox {
		virtual void config(int x, int y, GasesConfigsMap& configs, const Vector2i& size, const Vector2i& start) {
			if (y == size.y() - 1) {
				configs[0].boundary_T = 1.0;
				configs[1].boundary_T = 1.0;
			}
			if (x == 0) {
				configs[0].boundary_cond = sep::BT_MIRROR;
				configs[1].boundary_cond = sep::BT_MIRROR;
			}
		}
	};

	AddBox(Vector2d(0.0, 0.0), Vector2d(10.0, 10.0), new GPRT2Box());
}

double test_stream;

void GridConstructor::BoundaryConditionTest() {

	/*
	*	Boundary conditions test
	*/

	// normalization base
	Config::T_normalize = 600.0; // K
	Config::n_normalize = 1.81e22; // 1 / m^3
	Config::P_normalize = Config::n_normalize * sep::k * Config::T_normalize;
	Config::m_normalize = 133 * 1.66e-27; // kg
	Config::e_cut_normalize = sqrt(sep::k * Config::T_normalize / Config::m_normalize); // m / s
	Config::l_normalize = 1.0 * 6e-4; // m +
	Config::tau_normalize = Config::l_normalize / Config::e_cut_normalize; // s

	test_stream = 1.0e23 / (Config::n_normalize * Config::e_cut_normalize); // 1e23 1/(m^2 * s)

	PushTemperature(1.0);
	PushPressure(1.0);

	struct LeftStreamBox: GridBox {

		virtual void config(int x, int y, GasesConfigsMap& configs, const Vector2i& size, const Vector2i& start) {
			configs[0].pressure = 1.0;

			if (x == 0) {
				configs[0].boundary_cond = sep::BT_GASE;
				configs[0].boundary_stream = Vector3d(test_stream, 0.0, 0.0);
				configs[0].boundary_pressure = 1.0;
				configs[0].boundary_T = 1.0;
			}

			if (x == size.x() - 1) {
				configs[0].boundary_cond = sep::BT_GASE;
				configs[0].boundary_pressure = 1.0;
				configs[0].boundary_T = 1.0;
			}
		}
	};

	AddBox(Vector2d(0.0, 0.0), Vector2d(10.0, 10.0), new LeftStreamBox());

	struct RightPressureBox: GridBox {

		virtual void config(int x, int y, GasesConfigsMap& configs, const Vector2i& size, const Vector2i& start) {
			//configs[0].pressure = 0.5;

			if (x == 0) {
				configs[0].boundary_cond = sep::BT_GASE;
				configs[0].boundary_pressure = 1.0;
				configs[0].boundary_T = 1.0;
			}
		}
	};

	AddBox(Vector2d(12.0, 0.0), Vector2d(10.0, 10.0), new RightPressureBox());
}

void GridConstructor::PressureBoundaryConditionTestSmallArea() {

	/*
	*	Pressure boundary conditions test small area
	*/

	Config::vCellSize = Vector2d(0.1, 0.002); // mm

	// normalization base
	Config::T_normalize = 600.0; // K
	Config::n_normalize = 1.81e22; // 1 / m^3
	Config::P_normalize = Config::n_normalize * sep::k * Config::T_normalize;
	Config::m_normalize = 133 * 1.66e-27; // kg
	Config::e_cut_normalize = sqrt(sep::k * Config::T_normalize / Config::m_normalize); // m / s
	Config::l_normalize = 1.0e-3; // m
	Config::tau_normalize = Config::l_normalize / Config::e_cut_normalize; // s

	PushTemperature(1.0);
	PushPressure(1.0);

	struct LeftSimpleBox: GridBox {
		virtual void config(int x, int y, GasesConfigsMap& configs, const Vector2i& size, const Vector2i& start) {
			auto temp = [&](int x, int y, double& temperature) {
				temperature = ((double)y) / size.y() * (2.0 - 1.0) + 1.0;
			};

			if (x == 0) {
				temp(x, y, configs[0].boundary_T);
			}

			if (x == size.x() - 1) {
				temp(x, y, configs[0].boundary_T);
			}

			if (y == 0) {
				configs[0].boundary_T = 1.0;
			}

			if (y == size.y() - 1) {
				configs[0].boundary_T = 2.0;
			}
		}
	};

	AddBox(Vector2d(0.0, 0.0), Vector2d(5.0, 0.1), new LeftSimpleBox());

	struct RightPressureBox: GridBox {
		virtual void config(int x, int y, GasesConfigsMap& configs, const Vector2i& size, const Vector2i& start) {
			auto temp = [&](int x, int y, double& temperature) {
				temperature = ((double)y) / size.y() * (2.0 - 1.0) + 1.0;
			};

			if (x == 0) {
				configs[0].boundary_cond = sep::BT_GASE;
				configs[0].boundary_pressure = 1.0;
				temp(x, y, configs[0].boundary_T);
			}

			if (x == size.x() - 1) {
				configs[0].boundary_cond = sep::BT_GASE;
				configs[0].boundary_pressure = 1.0;
				temp(x, y, configs[0].boundary_T);
			}

			if (y == 0) {
				configs[0].boundary_T = 1.0;
			}

			if (y == size.y() - 1) {
				configs[0].boundary_T = 2.0;
			}
		}
	};

	AddBox(Vector2d(5.5, 0.0), Vector2d(5.0, 0.1), new RightPressureBox());
}

void GridConstructor::PressureBoundaryConditionTestBigArea() {
	/*
	*	Pressure boundary conditions large area
	*/

	Config::vCellSize = Vector2d(0.5, 0.01); // mm

	// normalization base
	Config::T_normalize = 600.0; // K
	Config::n_normalize = 1.81e22; // 1 / m^3
	Config::P_normalize = Config::n_normalize * sep::k * Config::T_normalize;
	Config::m_normalize = 133 * 1.66e-27; // kg
	Config::e_cut_normalize = sqrt(sep::k * Config::T_normalize / Config::m_normalize); // m / s
	Config::l_normalize = 1.0e-6; // m	!!!
	Config::tau_normalize = Config::l_normalize / Config::e_cut_normalize; // s

	PushTemperature(1.0);
	PushPressure(1.0);

	struct LeftSimpleBox: GridBox {

		virtual void config(int x, int y, GasesConfigsMap& configs, const Vector2i& size, const Vector2i& start) {
			auto temp = [&](int x, int y, double& temperature) {
				temperature = ((double)y) / size.y() * (2.0 - 1.0) + 1.0;
			};

			if (x == 0) {
				temp(x, y, configs[0].boundary_T);
			}

			if (x == size.x() - 1) {
				temp(x, y, configs[0].boundary_T);
			}

			if (y == 0) {
				configs[0].boundary_T = 1.0;
			}

			if (y == size.y() - 1) {
				configs[0].boundary_T = 2.0;
			}
		}
	};

	AddBox(Vector2d(0.0, 0.0), Vector2d(5.0, 0.1), new LeftSimpleBox());

	struct RightPressureBox: GridBox {

		virtual void config(int x, int y, GasesConfigsMap& configs, const Vector2i& size, const Vector2i& start) {
			auto temp = [&](int x, int y, double& temperature) {
				temperature = ((double)y) / size.y() * (2.0 - 1.0) + 1.0;
			};

			if (x == 0) {
				configs[0].boundary_cond = sep::BT_GASE;
				configs[0].boundary_pressure = 1.0;
				temp(x, y, configs[0].boundary_T);
			}

			if (x == size.x() - 1) {
				configs[0].boundary_cond = sep::BT_GASE;
				configs[0].boundary_pressure = 1.0;
				temp(x, y, configs[0].boundary_T);
			}

			if (y == 0) {
				configs[0].boundary_T = 1.0;
			}

			if (y == size.y() - 1) {
				configs[0].boundary_T = 2.0;
			}
		}
	};

	AddBox(Vector2d(5.5, 0.0), Vector2d(5.0, 0.1), new RightPressureBox());
}
