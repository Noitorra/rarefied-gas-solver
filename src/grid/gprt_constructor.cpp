#include "grid_constructor.h"
#include "config.h"
#include "parameters/beta_chain.h"

// TODO: solve this problem of lambdas
double T1, T2, P_sat_T1, P_sat_T2, Q_Xe_in, P_sat_Xe;

void GridConstructor::ConfigureGPRT() {
    //    Config::vCellSize = Vector2d(8.0, 0.25);
    //Config::vCellSize = Vector2d(3.0, 0.4);
    Config::vCellSize = Vector2d(8.0, 0.4);	// mm
    T1 = 325.0 + 273.0;
    T2 = 60.0 + 273.0;
    Vector2d walls(Config::vCellSize);
    Vector2d sp_delta(Config::vCellSize);
//    sp_delta /= 2.0;
    walls *= 2.0;

    // normalization base
    Config::T_normalize = 600.0;  // K
    Config::n_normalize = 1.81e22;  // 1 / m^3
	Config::P_normalize = Config::n_normalize * sep::k * Config::T_normalize;
    Config::m_normalize = 133 * 1.66e-27;   // kg
    Config::e_cut_normalize = sqrt(sep::k * Config::T_normalize / Config::m_normalize); // m / s
    //Config::l_normalize = 2.78e-5; // m
	//Config::l_normalize = 6e-5; // m -	Diman count it
	Config::l_normalize = 0.5 * 6e-4; // m +
	//Config::l_normalize = 0.2 * 6e-4; // m -
    Config::tau_normalize = Config::l_normalize / Config::e_cut_normalize;  // s

    T1 /= Config::T_normalize;
    T2 /= Config::T_normalize;
    PushTemperature(T1);
    P_sat_T1 = 1.0; // 150Pa, T = T0, n = n0
    P_sat_T2 = 2.7e-4 / Config::P_normalize;	// 2.7 x 10^-4 Pa при 320 K
    Q_Xe_in = 2.46e-9;
    P_sat_Xe = P_sat_T1 * 3 * 1e-7;	//	P_sat_T1 * атом. доля (растворимость Xe в жидком Cs)

	
	// Some beta chain hack. Makes lambda without data type.
	for (auto& item : Config::vBetaChains) {
		item->dLambda1 *= Config::tau_normalize;
		item->dLambda2 *= Config::tau_normalize;
	}

    auto global_temp = [&] (Vector2d p, double& temperature) {        
//        std::cout << "T1 = " << T1 << " " << T2 << std::endl;
        const double sp_delta_wall = 30.0;
        const double temp_start = 100.0 + sp_delta_wall;
        const double temp_finish = 380.0;
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
    auto global_pressure_Cs = [=] (Vector2d p, double& pressure) {
        pressure = (1.0 - (p.x() - 30.0) / 400.0) * P_sat_T1;
        //std::cout << "P = " << pressure << std::endl;
    };
	auto global_pressure_Xe = [=] (Vector2d p, double& pressure) {
		pressure = (1.0 - (p.x() - 30.0) / 400.0) * P_sat_Xe;
	};

    // boxes ========================================================================
    // box 1
//    PushPressure(1.0);
    PushPressure(P_sat_T1);
    SetBox(Vector2d(-20.0, 0.0), Vector2d(150.0 - sp_delta.x(), 4.0),
            [] (int x, int y, GasesConfigsMap& configs, const Vector2i& size, const Vector2i& start) {

                configs[1].pressure = P_sat_Xe;

                if (x == 0) {
                    configs[0].boundary_cond = sep::BT_PRESSURE;
                    configs[0].boundary_pressure = P_sat_T1;
                    configs[0].boundary_T = T1;

                    configs[1].boundary_cond = sep::BT_STREAM;
                    configs[1].boundary_T = T1;
                    configs[1].boundary_stream = Vector3d(Q_Xe_in, 0.0, 0.0);
                }

//                if (y == 0) {
//                    for (int gas = 0; gas < Config::iGasesNumber; gas++) {
//                        configs[gas].boundary_cond = sep::BT_MIRROR;
//                    }
//                }
            });

    // box 2
//    PushPressure(0.95);
    SetBox(Vector2d(100.0, 0.0), Vector2d(30.0, 12.5),
            [] (int x, int y, GasesConfigsMap& configs, const Vector2i& size, const Vector2i& start) {

                configs[1].pressure = P_sat_Xe;

//                if (y == 0) {
//                    for (int gas = 0; gas < Config::iGasesNumber; gas++) {
//                        configs[gas].boundary_cond = sep::BT_MIRROR;
//                    }
//                }
            });

    // box 3
//    PushPressure(0.9);
    SetBox(Vector2d(30.0, 11.25), Vector2d(100.0, 1.25),
            [] (int x, int y, GasesConfigsMap& configs, const Vector2i& size, const Vector2i& start) {

            configs[1].pressure = P_sat_Xe;

            if (x == 0) {
                configs[0].boundary_cond = sep::BT_PRESSURE;    // should be gas <-> fluid bound
                configs[0].boundary_pressure = P_sat_T1;
                configs[0].boundary_T = T1;

                configs[1].boundary_cond = sep::BT_PRESSURE;    // should be adsorption
                configs[1].boundary_pressure = P_sat_Xe;
                configs[1].boundary_T = T1;
            }
    });

    // box 4
//    PushPressure(0.85);
    SetBox(Vector2d(30.0, 15.5), Vector2d(100.0 + walls.x(), 2.5),
            [&] (int x, int y, GasesConfigsMap& configs, const Vector2i& size, const Vector2i& start) {

            Vector2i p_abs(x + start.x(), y + start.y());
			Vector2d p(p_abs.x() * Config::vCellSize.x(), p_abs.y() * Config::vCellSize.y());
            
			global_temp(p, configs[0].T);
            global_temp(p, configs[0].boundary_T);
            global_pressure_Cs(p, configs[0].pressure);

			global_temp(p, configs[1].T);
			global_temp(p, configs[1].boundary_T);
            global_pressure_Xe(p, configs[1].pressure);

            if (x == 0) {
                configs[0].boundary_cond = sep::BT_PRESSURE;    // should be fluid <-> gas bound
                configs[0].boundary_pressure = P_sat_T1;
                configs[0].boundary_T = T1;

                configs[1].boundary_cond = sep::BT_PRESSURE;    // should be adsorption
                configs[1].boundary_pressure = P_sat_Xe;
                configs[1].boundary_T = T1;
            }
    });

    // box 5
//    PushPressure(0.8);
    SetBox(Vector2d(130.0 + walls.x() - sp_delta.x(), 0.0), Vector2d(200.0 - walls.x(), 18.0 - sp_delta.y()),
            [&] (int x, int y, GasesConfigsMap& configs, const Vector2i& size, const Vector2i& start) {

			Vector2i p_abs(x + start.x(), y + start.y());
			Vector2d p(p_abs.x() * Config::vCellSize.x(), p_abs.y() * Config::vCellSize.y());
            
			global_temp(p, configs[0].T);
            global_temp(p, configs[0].boundary_T);
            global_pressure_Cs(p, configs[0].pressure);

			global_temp(p, configs[1].T);
			global_temp(p, configs[1].boundary_T);
			global_pressure_Xe(p, configs[1].pressure);

//            if (y == 0) {
//                for (int gas = 0; gas < Config::iGasesNumber; gas++) {
//                    configs[gas].boundary_cond = sep::BT_MIRROR;
//                }
//            }
    });

    // box 6
//    PushPressure(0.75);
    SetBox(Vector2d(330.0 - sp_delta.x(), 0.0), Vector2d(100.0, 8.0),
            [&] (int x, int y, GasesConfigsMap& configs, const Vector2i& size, const Vector2i& start) {

				Vector2i p_abs(x + start.x(), y + start.y());
				Vector2d p(p_abs.x() * Config::vCellSize.x(), p_abs.y() * Config::vCellSize.y());
                
				global_temp(p, configs[0].T);
                global_temp(p, configs[0].boundary_T);
                global_pressure_Cs(p, configs[0].pressure);

				global_temp(p, configs[1].T);
				global_temp(p, configs[1].boundary_T);
				global_pressure_Xe(p, configs[1].pressure);

                if (x == size.x() - 1) {
                    for (int gas = 0; gas < Config::iGasesNumber; gas++) {
                        configs[gas].boundary_cond = sep::BT_PRESSURE;
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
                if (y == size.y() - 1 && p.x() > 380.0) {
                    configs[0].boundary_cond = sep::BT_PRESSURE;
                    configs[0].boundary_pressure = P_sat_T2;
                    configs[0].boundary_T = T2;
                }
            });

    // box 7
//    PushPressure(0.7);
    SetBox(Vector2d(130.0 - sp_delta.x(), 0.0), Vector2d(walls.x(), Config::vCellSize.y() * 4),
            [&] (int x, int y, GasesConfigsMap& configs, const Vector2i& size, const Vector2i& start) {

                configs[0].pressure = (1.0 - (double)x / size.x() * 0.38) * P_sat_T1;
				configs[1].pressure = (1.0 - (double)x / size.x() * 0.38) * P_sat_Xe;
				
				Vector2i p_abs(x + start.x(), y + start.y());
				Vector2d p(p_abs.x() * Config::vCellSize.x(), p_abs.y() * Config::vCellSize.y());
                
				global_temp(p, configs[0].T);
                global_temp(p, configs[0].boundary_T);

				global_temp(p, configs[1].T);
				global_temp(p, configs[1].boundary_T);

//                if (y == 0) {
//                    for (int gas = 0; gas < Config::iGasesNumber; gas++) {
//                        configs[gas].boundary_cond = sep::BT_MIRROR;
//                    }
//                }
            });
}

void GridConstructor::ConfigureGPRT2() {

    PushTemperature(0.5);
    SetBox(Vector2d(0.0, 0.0), Vector2d(10.0, 10.0),
        [] (int x, int y, GasesConfigsMap& configs, const Vector2i& size, const Vector2i& start) {

            if (y == size.y() - 1) {
                configs[0].boundary_T = 1.0;
                configs[1].boundary_T = 1.0;
            }
            if (x == 0) {
                configs[0].boundary_cond = sep::BT_MIRROR;
                configs[1].boundary_cond = sep::BT_MIRROR;
            }
        });
}

void GridConstructor::BoundaryConditionTest() {

	/*
	*	Boundary conditions test
	*/

	// normalization base
	Config::T_normalize = 600.0;  // K
	Config::n_normalize = 1.81e22;  // 1 / m^3
	Config::P_normalize = Config::n_normalize * sep::k * Config::T_normalize;
	Config::m_normalize = 133 * 1.66e-27;   // kg
	Config::e_cut_normalize = sqrt(sep::k * Config::T_normalize / Config::m_normalize); // m / s
	Config::l_normalize = 2.78e-5; // m
	Config::tau_normalize = Config::l_normalize / Config::e_cut_normalize;  // s

	double test_stream = 1.0e15 / (Config::n_normalize * Config::e_cut_normalize);	// 1e15 1/(m^2 * s)
	double test_pressure = 100.0 / Config::P_normalize;	// 100 Pa

	PushTemperature(1.0);
	PushPressure(1.0);
	// Lef box for testing stream
	SetBox(Vector2d(0.0, 0.0), Vector2d(10.0, 10.0),
		[=] (int x, int y, GasesConfigsMap& configs, const Vector2i& size, const Vector2i& start) {
			
			configs[0].pressure = 0.0;

			if (x == 0) {
				configs[0].boundary_cond = sep::BT_STREAM;
				configs[0].boundary_stream = Vector3d(test_stream, 0.0, 0.0);
				configs[0].boundary_T = 1.0;
			}

			if (x == size.x() - 1) {
				configs[0].boundary_cond = sep::BT_PRESSURE;
				configs[0].boundary_pressure = 0.0;
				configs[0].boundary_T = 1.0;
			}
	});

	// Right box for testing pressure
	SetBox(Vector2d(12.0, 0.0), Vector2d(10.0, 10.0),
		[=] (int x, int y, GasesConfigsMap& configs, const Vector2i& size, const Vector2i& start) {

			configs[0].pressure = test_pressure / 2.0;

			if (x == 0) {
				configs[0].boundary_cond = sep::BT_PRESSURE;
				configs[0].boundary_pressure = test_pressure;
				configs[0].boundary_T = 1.0;
			}
	});
}

void GridConstructor::PressureBoundaryConditionTestSmallArea() {

	/*
	*	Pressure boundary conditions test small area
	*/

	Config::vCellSize = Vector2d(0.1, 0.002);	// mm

	// normalization base
	Config::T_normalize = 600.0;  // K
	Config::n_normalize = 1.81e22;  // 1 / m^3
	Config::P_normalize = Config::n_normalize * sep::k * Config::T_normalize;
	Config::m_normalize = 133 * 1.66e-27;   // kg
	Config::e_cut_normalize = sqrt(sep::k * Config::T_normalize / Config::m_normalize); // m / s
	Config::l_normalize = 1.0e-3; // m
	Config::tau_normalize = Config::l_normalize / Config::e_cut_normalize;  // s

	PushTemperature(1.0);
	PushPressure(1.0);

	// Left simple box
	SetBox(Vector2d(0.0, 0.0), Vector2d(5.0, 0.1),
		[=] (int x, int y, GasesConfigsMap& configs, const Vector2i& size, const Vector2i& start) {

			auto temp = [&] (int x, int y, double& temperature) {
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
	});

	// Right box with pressure
	SetBox(Vector2d(5.5, 0.0), Vector2d(5.0, 0.1),
		[=] (int x, int y, GasesConfigsMap& configs, const Vector2i& size, const Vector2i& start) {

			auto temp = [&] (int x, int y, double& temperature) {
				temperature = ((double)y) / size.y() * (2.0 - 1.0) + 1.0;
			};

			if (x == 0) {
				configs[0].boundary_cond = sep::BT_PRESSURE;
				configs[0].boundary_pressure = 1.0;
				temp(x, y, configs[0].boundary_T);
			}

			if (x == size.x() - 1) {
				configs[0].boundary_cond = sep::BT_PRESSURE;
				configs[0].boundary_pressure = 1.0;
				temp(x, y, configs[0].boundary_T);
			}

			if (y == 0) {
				configs[0].boundary_T = 1.0;
			}

			if (y == size.y() - 1) {
				configs[0].boundary_T = 2.0;
			}
	});
}

void GridConstructor::PressureBoundaryConditionTestBigArea() {
	/*
	*	Pressure boundary conditions large area
	*/

	Config::vCellSize = Vector2d(0.5, 0.01);	// mm

	// normalization base
	Config::T_normalize = 600.0;  // K
	Config::n_normalize = 1.81e22;  // 1 / m^3
	Config::P_normalize = Config::n_normalize * sep::k * Config::T_normalize;
	Config::m_normalize = 133 * 1.66e-27;   // kg
	Config::e_cut_normalize = sqrt(sep::k * Config::T_normalize / Config::m_normalize); // m / s
	Config::l_normalize = 1.0e-6; // m	!!!
	Config::tau_normalize = Config::l_normalize / Config::e_cut_normalize;  // s

	PushTemperature(1.0);
	PushPressure(1.0);

	// Left simple box
	SetBox(Vector2d(0.0, 0.0), Vector2d(5.0, 0.1),
		[=] (int x, int y, GasesConfigsMap& configs, const Vector2i& size, const Vector2i& start) {

			auto temp = [&] (int x, int y, double& temperature) {
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
	});

	// Right box with pressure
	SetBox(Vector2d(5.5, 0.0), Vector2d(5.0, 0.1),
		[=] (int x, int y, GasesConfigsMap& configs, const Vector2i& size, const Vector2i& start) {

			auto temp = [&] (int x, int y, double& temperature) {
				temperature = ((double)y) / size.y() * (2.0 - 1.0) + 1.0;
			};

			if (x == 0) {
				configs[0].boundary_cond = sep::BT_PRESSURE;
				configs[0].boundary_pressure = 1.0;
				temp(x, y, configs[0].boundary_T);
			}

			if (x == size.x() - 1) {
				configs[0].boundary_cond = sep::BT_PRESSURE;
				configs[0].boundary_pressure = 1.0;
				temp(x, y, configs[0].boundary_T);
			}

			if (y == 0) {
				configs[0].boundary_T = 1.0;
			}

			if (y == size.y() - 1) {
				configs[0].boundary_T = 2.0;
			}
	});
}

