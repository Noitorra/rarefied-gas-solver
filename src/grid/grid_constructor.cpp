#include "grid_constructor.h"
#include "config.h"
#include "parameters/beta_chain.h"

#define M_PI           3.14159265358979323846  /* pi */


struct EmitterPart {
	EmitterPart(double length, bool isAHole, double temp) {
		this->length = length;
		this->isAHole = isAHole;
		this->temp = temp;
	}
	double length;
	bool isAHole;
	double temp;
};

class Emitter {
public:
	static Emitter* getInstance() {
		static Emitter* pInstance = new Emitter();
		return pInstance;
	}

	void fillEmitter() {
		m_vEmitterParts.push_back(new EmitterPart(86, false, (1200.0 + 1475.0) / 2));
		m_vEmitterParts.push_back(new EmitterPart(10.8, true, (1475.0 + 1340.0) / 2));
		m_vEmitterParts.push_back(new EmitterPart(45.5, false, (1340.0 + 1480.0) / 2));
		m_vEmitterParts.push_back(new EmitterPart(10.8, true, (1480.0 + 1350.0) / 2));
		m_vEmitterParts.push_back(new EmitterPart(36.5, false, (1350.0 + 1480.0) / 2));
		m_vEmitterParts.push_back(new EmitterPart(10.8, true, (1480.0 + 1350.0) / 2));
		m_vEmitterParts.push_back(new EmitterPart(32.2, false, (1350.0 + 1460.0) / 2));
		m_vEmitterParts.push_back(new EmitterPart(10.8, true, (1460.0 + 1360.0) / 2));
		m_vEmitterParts.push_back(new EmitterPart(32.2, false, (1360.0 + 1480.0) / 2));
		m_vEmitterParts.push_back(new EmitterPart(10.8, true, (1480.0 + 1350.0) / 2));
		m_vEmitterParts.push_back(new EmitterPart(32.2, false, (1350.0 + 1460.0) / 2));
		m_vEmitterParts.push_back(new EmitterPart(10.8, true, (1460.0 + 1350.0) / 2));
		m_vEmitterParts.push_back(new EmitterPart(36.5, false, (1350.0 + 1475.0) / 2));
		m_vEmitterParts.push_back(new EmitterPart(10.8, true, (1475.0 + 1350.0) / 2));
		m_vEmitterParts.push_back(new EmitterPart(45.5, false, (1350.0 + 1365.0) / 2));
		m_vEmitterParts.push_back(new EmitterPart(11.8, true, (1365.0 + 1320.0) / 2));
		m_vEmitterParts.push_back(new EmitterPart(86.0, false, (1320.0 + 1400.0) / 2));

		// Get Rid of Kelvin
		for (auto& item : m_vEmitterParts) {
			item->temp /= Config::T_normalize;
		}

		_calc_length();
	}

	const double getLength() const {
		return m_dLength;
	}

	EmitterPart* getEmitterPart(double dBegin, double dEnd) {
		// get all parts within beg end cell
		std::vector<EmitterPart*> parts;
		double l = 0.0; // already calc
		for (auto& item : m_vEmitterParts) {
			if ((dBegin + dEnd) / 2 * m_dLength > l && (dEnd + dBegin) / 2 * m_dLength <= l + item->length ||
				(l + item->length / 2) > dBegin * m_dLength && (l + item->length / 2) <= dEnd * m_dLength)
			{
				parts.push_back(item);
			}
			l += item->length;
		}

		//std::cout << parts.size() << std::endl;

		m_pEmitterPart = nullptr;
		for (auto& item : parts) {
			if (m_pEmitterPart == nullptr) {
				m_pEmitterPart = item;
			}
			else {
				m_pEmitterPart->temp += item->temp;
				if (item->isAHole) {
					m_pEmitterPart->isAHole = true;
				}
			}
		}

		if (m_pEmitterPart && !parts.empty()) {
			m_pEmitterPart->temp /= parts.size();

			std::cout << m_pEmitterPart->isAHole << std::endl;
		}
		else {
			std::cout << "Error" << std::endl;
		}

		return m_pEmitterPart;
	}
private:
	Emitter() {
		m_pEmitterPart = nullptr;
	}

	void _calc_length() {
		m_dLength = 0.0;
		for (auto& item : m_vEmitterParts) {
			m_dLength += item->length;
		}
	}

	std::vector<EmitterPart*> m_vEmitterParts;
	double m_dLength;
	EmitterPart* m_pEmitterPart;
};

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
    Config::l_normalize = 6.07e-5; // эффективная длинна свободного пробега
    Config::tau_normalize = Config::l_normalize / Config::e_cut_normalize;  // s

		// Some beta chain hack. Makes lambda without data type.
		for (auto& item : Config::vBetaChains) {
			item->dLambda1 *= Config::tau_normalize;
			item->dLambda2 *= Config::tau_normalize;
		}
    
		// Bottom emmiter holes positioning
		Emitter::getInstance()->fillEmitter();
		std::cout << "Emmiter length = " << Emitter::getInstance()->getLength() << std::endl;

    PushTemperature(1.0);
    PushPressure(1.0);

    //Vector2d vPhysSize = Vector2d(520.0, 0.4);
		Vector2d vPhysSize = Vector2d(5.2, 0.4);
    Vector2i vNumSize = Vector2i(50, 10);
    Config::vCellSize = Vector2d(vPhysSize.x() / vNumSize.x(), vPhysSize.y() / vNumSize.y());

		SetBox(Vector2d(0.0, 0.0), vPhysSize, [](int x, int y, GasesConfigsMap& configs,
			const Vector2i& size, const Vector2i& start) 
		{
			// Task parameters
			double T1 = 1500.0 / Config::T_normalize;
			double T2 = 900.0 / Config::T_normalize;

			double dPCsLeft = 150.0 / Config::P_normalize;
			double dPCsRight = 150.0 / Config::P_normalize;

			double dPKr = 7e-7;
			double dPXe = 1.2e-6;

			double dS = M_PI * (4e-3 * 4e-3);

			double dSpeedKr = 2.1e-9 / (dPKr * dS) / Config::e_cut_normalize; // Q/(P*S) S = pi * D^2 / 4
			double dSpeedXe = 3.6e-9 / (dPXe * dS) / Config::e_cut_normalize; // Q/(P*S) S = pi * D^2 / 4

			//dSpeedKr *= 10; // 5 10
			//dSpeedXe *= 10; // 5 10

			dPKr = dPKr / Config::P_normalize;
			dPXe = dPXe / Config::P_normalize;

			// support variables
			double dTgrad = T1 - (T1 - T2) * (2 * (y - 1) + 1) / 2 / (size.y() - 2);
			double dPgrad = dPCsLeft - (dPCsLeft - dPCsRight) * (2 * (x - 1) + 1) / 2 / (size.x() - 2);

			//configs[0].pressure = dPCsLeft;
			configs[0].pressure = dPgrad;
			configs[0].T = dTgrad;

			configs[1].pressure = 0.0; // hack
			configs[1].T = dTgrad;

			configs[2].pressure = 0.0; // hack
			configs[2].T = dTgrad;

			for (int i = 1; i < Config::iGasesNumber; i++) {
				configs[i].pressure = 0.0;
				configs[i].T = dTgrad;
			}

			if (x == 0) { // left border
				configs[0].boundary_cond = sep::BT_GASE;
				configs[0].boundary_pressure = dPCsLeft;
				configs[0].boundary_T = dTgrad;

				for (int i = 1; i < Config::iGasesNumber; i++) {
					configs[i].boundary_cond = sep::BT_DIFFUSE;
					configs[i].boundary_T = dTgrad;
				}
			}
			if (x == size.x() - 1) { // right border
				configs[0].boundary_cond = sep::BT_GASE;
				configs[0].boundary_pressure = dPCsRight; // dPCsRight
				configs[0].boundary_T = dTgrad;

				for (int i = 1; i < Config::iGasesNumber; i++) {
					configs[i].boundary_cond = sep::BT_GASE;
					configs[i].boundary_pressure = 0.0;
					configs[i].boundary_stream = Vector3d();
					configs[i].boundary_T = dTgrad;
				}
			}
			if (y == 0) { // bottom border
				double dBegin = (1.0 * (x - 1)) / (size.x() - 2);
				double dEnd = (1.0 * (x)) / (size.x() - 2);
				EmitterPart* e = Emitter::getInstance()->getEmitterPart(dBegin, dEnd);
				if (e && e->isAHole) { // a hole
					configs[0].boundary_cond = sep::BT_DIFFUSE;
					configs[0].boundary_T = e->temp;

					// Kr
					configs[1].boundary_cond = sep::BT_FLOW;
					configs[1].boundary_pressure = dPKr;
					configs[1].boundary_stream = Vector3d(0.0, dSpeedKr * dPKr / e->temp, 0.0);
					configs[1].boundary_T = e->temp;

					// Xe
					configs[2].boundary_cond = sep::BT_FLOW;
					configs[2].boundary_pressure = dPXe;
					configs[2].boundary_stream = Vector3d(0.0, dSpeedXe* dPKr / e->temp, 0.0);
					configs[2].boundary_T = e->temp;

					for (int i = 3; i < Config::iGasesNumber; i++) {
						configs[i].boundary_cond = sep::BT_DIFFUSE;
						configs[i].boundary_T = e->temp;
					}
				}
				else {
					configs[0].boundary_cond = sep::BT_DIFFUSE;
					configs[0].boundary_T = e ? e->temp : T1;

					for (int i = 1; i < Config::iGasesNumber; i++) {
						configs[i].boundary_cond = sep::BT_DIFFUSE;
						configs[i].boundary_T = e ? e->temp : T1;
					}
				}
			}
			if (y == size.y() - 1) { // top border
				configs[0].boundary_cond = sep::BT_DIFFUSE;
				configs[0].boundary_T = T2;

				for (int i = 1; i < Config::iGasesNumber; i++) {
					configs[i].boundary_cond = sep::BT_DIFFUSE;
					configs[i].boundary_T = T2;
				}
			}
			
		});
}
