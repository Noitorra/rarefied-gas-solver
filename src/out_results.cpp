#include "out_results.h"
#include "grid/grid.h"
#include "grid/grid_manager.h"
#include "grid/cell.h"
#include "config.h"

#ifdef _WIN32
#include <windows.h>
#endif

std::string param_to_str(sep::MacroParamType param) {
	switch (param) {
	case sep::T_PARAM:
		return "temp";
	case sep::C_PARAM:
		return "conc";
	case sep::P_PARAM:
		return "pressure";
	case sep::FLOW_PARAM:
		return "flow";
	default:
		return "no_param";
	}
}

void mkdir(const std::string& dir) {
#ifdef _WIN32
	if (CreateDirectory(dir.c_str(), NULL)) {
		// Directory created
	} else if (ERROR_ALREADY_EXISTS == GetLastError()) {
		// Directory already exists
	} else {
		// Failed for some other reason
	}
#else
  system(("mkdir " + dir).c_str());
#endif
}

void rmdir(const std::string& dir) {
#ifdef _WIN32
	SHFILEOPSTRUCT file_op = {
		NULL,
		FO_DELETE,
		dir.c_str(),
		"",
		FOF_NOCONFIRMATION |
		FOF_NOERRORUI |
		FOF_SILENT ,
		false,
		0,
		""};
	SHFileOperation(&file_op);
#else
  system(("rm -rf " + dir).c_str());
#endif
}

void OutResults::Init(Grid* grid, GridManager* grid_manager) {
	grid_ = grid;
	grid_manager_ = grid_manager;

	std::string out_dir = Config::sOutputPrefix + "out";
	rmdir(out_dir);
	mkdir(out_dir);

	// check if needed to create directories
	for (int gas = 0; gas < Config::iGasesNumber; gas++) {
		std::string gas_dir = out_dir + "/gas" + to_string(gas);
		mkdir(gas_dir);

		for (int param = 0; param < (int)sep::LAST_PARAM; param++) {
			std::string param_dir = gas_dir + "/" + param_to_str((sep::MacroParamType)param);

			mkdir(param_dir);
			mkdir(param_dir + "/data");
			mkdir(param_dir + "/pic");
		}
	}
}

void OutResults::OutAll(int iteration) {
	if (!grid_ || !grid_manager_) {
		std::cout << "Error: member OutResults is not initialized yet" << std::endl;
		return;
	}
	if (iteration % Config::iOutEach)
		return;
	std::cout << "Out results " << iteration;

	LoadParameters();

	tbb::parallel_for(tbb::blocked_range<int>(0, sep::LAST_PARAM), [&](const tbb::blocked_range<int>& r) {
		                  for (int param = r.begin(); param != r.end(); ++param) {
			                  for (int gas = 0; gas < Config::iGasesNumber; gas++) {
				                  OutParameter((sep::MacroParamType)param, gas, iteration);
			                  }
		                  }
		                  std::cout << ".";
	                  });

	std::cout << std::endl;
	//  OutAverageStream(iteration);
}

// prepare parameters to be printed out
void OutResults::LoadParameters() {
	std::vector<std::vector<std::vector<InitCellData*>>>& cells = grid_->GetInitCells();

	const Vector3i& vSize = grid_->GetSize();
	tbb::parallel_for(tbb::blocked_range<int>(0, vSize.x()), [&](const tbb::blocked_range<int>& r) {
		                  for (int x = r.begin(); x != r.end(); ++x) {
			                  for (int y = 0; y < vSize.y(); y++) {
				                  const int z = 0;
				                  if (cells[x][y][z]->m_eType != sep::NORMAL_CELL)
					                  continue;
				                  Cell* cell = cells[x][y][z]->m_pCell;
				                  cell->computeMacroData();
			                  }
		                  }
	                  });
}

void OutResults::OutParameter(sep::MacroParamType type, int gas, int index) {
	std::vector<std::vector<std::vector<InitCellData*>>>& cells = grid_->GetInitCells();

	std::string filename;
	const std::string& output_prefix = Config::sOutputPrefix;

	filename = output_prefix + "out/gas" + to_string(gas) +
			"/" + param_to_str(type) + "/data/" + to_string(index) + ".bin";

	std::ofstream fs(filename.c_str(), std::ios::out | std::ios::binary);

	const Vector3i& grid_size = Config::vGridSize;

	// writing additional info
	double tmp = (double)grid_size.x();
	fs.write(reinterpret_cast<const char*>(&tmp), sizeof(double));
	tmp = (double)grid_size.y();
	fs.write(reinterpret_cast<const char*>(&tmp), sizeof(double));

	int params_num = type == sep::FLOW_PARAM ? 2 : 1;
	std::vector<double> params(params_num);
	for (int y = 0; y < grid_size.y(); y++) {
		for (int x = 0; x < grid_size.x(); x++) {
			int z = 0;

			Cell* cell = cells[x][y][z]->m_pCell;
			if (cell && cells[x][y][z]->m_eType == sep::NORMAL_CELL) {

				// from normalized values back to normal
				const double t = cell->m_vMacroData[gas].T * Config::T_normalize;
				const double n = cell->m_vMacroData[gas].C * Config::n_normalize;
				const double p = cell->m_vMacroData[gas].P * Config::P_normalize;
				const double stream_x = cell->m_vMacroData[gas].Stream.x() * Config::n_normalize * Config::e_cut_normalize;
				const double stream_y = cell->m_vMacroData[gas].Stream.y() * Config::n_normalize * Config::e_cut_normalize;

				switch (type) {
				case sep::T_PARAM:
					params[0] = t;
					break;
				case sep::C_PARAM:
					params[0] = n;
					break;
				case sep::P_PARAM:
					params[0] = p;
					break;
				case sep::FLOW_PARAM:
					params[0] = stream_x;
					params[1] = stream_y;
					break;
				default:
					return;
				}
			} else {
				std::fill(params.begin(), params.end(), 0.0);
			}

			fs.write(reinterpret_cast<const char*>(&params[0]), sizeof(double) * params.size());
		}
	}
	fs.close();
}

void OutResults::OutAverageStream(int iteration) {
	GasVector& gasv = Config::vGas;

	const std::string& output_prefix = Config::sOutputPrefix;
	std::string as_filename_base = output_prefix + "out/gas";
	for (unsigned int gi = 0; gi < Config::iGasesNumber; gi++) {
		std::string as_filename = as_filename_base + to_string(gi);
		as_filename += "/";
		as_filename += "average_stream.bin";

		std::fstream filestream;
		std::ios::openmode openmode;
		if (iteration == 0) {
			openmode = std::ios::out | std::ios::binary;
		} else {
			openmode = std::ios::app | std::ios::binary;
		}
		filestream.open(as_filename.c_str(), openmode);
		if (filestream.is_open()) {

			if (Config::bGPRTGrid == true) {
				OutAverageStreamGPRT(filestream, gi);
			} else {
				OutAverageStreamComb(filestream, gi);
			}

			filestream.close();
		} else {
			std::cout << "OutResults::OutAverageStream() : Cannot open file - " << as_filename << std::endl;
		}
	}
}

void OutResults::OutAverageStreamComb(std::fstream& filestream, int gas_n) {
	const Vector3i& grid_size = grid_->GetSize();
	double left_average_stream = ComputeAverageColumnStream(1, gas_n, 0, grid_size.y());
	double right_average_stream = ComputeAverageColumnStream(grid_size.x() - 2, gas_n, 0, grid_size.y());

	filestream.write(reinterpret_cast<const char*>(&left_average_stream), sizeof(double));
	filestream.write(reinterpret_cast<const char*>(&right_average_stream), sizeof(double));

	std::cout << left_average_stream << " : " << right_average_stream << std::endl;
}

void OutResults::OutAverageStreamGPRT(std::fstream& filestream, int gas_n) {
	const Vector3i& grid_size = grid_->GetSize();
	HTypeGridConfig* h_type_config = Config::pHTypeGridConfig;
	int shift_x = 5;
	int D = h_type_config->D;
	int l = h_type_config->l;

	double left_up_stream = ComputeAverageColumnStream(shift_x, gas_n, 1, D - 2);
	double right_up_stream = ComputeAverageColumnStream(grid_size.x() - 1 - shift_x, gas_n, 1, D - 2);
	double left_down_stream = ComputeAverageColumnStream(shift_x, gas_n, D + l + 1, D - 2);
	double right_down_stream = ComputeAverageColumnStream(grid_size.x() - 1 - shift_x, gas_n, D + l + 1, D - 2);

	filestream.write(reinterpret_cast<const char*>(&left_up_stream), sizeof(double));
	filestream.write(reinterpret_cast<const char*>(&right_up_stream), sizeof(double));
	filestream.write(reinterpret_cast<const char*>(&left_down_stream), sizeof(double));
	filestream.write(reinterpret_cast<const char*>(&right_down_stream), sizeof(double));

	std::cout << left_up_stream << " : " << right_up_stream << " : " <<
			left_down_stream << " : " << right_down_stream << std::endl;
}

double OutResults::ComputeAverageColumnStream(int index_x, unsigned int gi, int start_y, int size_y) {
	std::vector<std::vector<std::vector<InitCellData*>>>& m_vCells = grid_->GetInitCells();

	double average_stream = 0.0;
	for (int y = start_y; y < start_y + size_y; y++) {
		average_stream += m_vCells[index_x][y][0]->m_vMacroData[gi].Stream.x();
	}

	average_stream /= size_y;

	return average_stream;
}
