#include <boost/filesystem.hpp>
#include "out_results.h"
#include "grid/grid.h"
#include "grid/grid_manager.h"
#include "grid/cell.h"
#include "config/config.h"
#include "config/normalizer.h"

std::string param_to_str(sep::MacroParamType param) {
    switch (param) {
        case sep::T_PARAM:
            return "temperature";
        case sep::C_PARAM:
            return "density";
        case sep::P_PARAM:
            return "pressure";
        case sep::FLOW_PARAM:
            return "flow";
        default:
            return "undefined";
    }
}

OutResults::OutResults() : m_pGrid(nullptr), m_pGridManager(nullptr), m_pConfig(Config::getInstance()) {}

void OutResults::Init(Grid* pGrid, GridManager* pGridManager) {
    m_pGrid = pGrid;
    m_pGridManager = pGridManager;

    std::string sOutDir = m_pConfig->getOutputPrefix() + "out";

    if (boost::filesystem::exists(sOutDir)) {
        boost::filesystem::remove_all(sOutDir);
    }
    boost::filesystem::create_directory(sOutDir);

    // check if needed to create directories
    for (int iGas = 0; iGas < m_pConfig->getGasesNum(); iGas++) {
        std::string sGasDir = sOutDir + "/" + "gas" + to_string(iGas);
        boost::filesystem::create_directory(sGasDir);

        for (int iParam = 0; iParam < static_cast<int>(sep::LAST_PARAM); iParam++) {
            std::string param_dir = sGasDir + "/" + param_to_str(static_cast<sep::MacroParamType>(iParam));

            boost::filesystem::create_directory(param_dir);
            boost::filesystem::create_directory(param_dir + "/" + "data");
            boost::filesystem::create_directory(param_dir + "/" + "pic");
        }
    }
}

void OutResults::OutAll(int iIteration) {
    if (m_pGrid == nullptr || m_pGridManager == nullptr) {
        std::cout << "Error: member OutResults is not initialized yet" << std::endl;
        return;
    }
    if (iIteration % m_pConfig->getOutEach()) {
        return;
    }
    std::cout << "Out results " << iIteration;

    LoadParameters();

    tbb::parallel_for(tbb::blocked_range<int>(0, sep::LAST_PARAM), [&](const tbb::blocked_range<int>& r) {
        for (int param = r.begin(); param != r.end(); ++param) {
            for (int gas = 0; gas < m_pConfig->getGasesNum(); gas++) {
                OutParameter(static_cast<sep::MacroParamType>(param), gas, iIteration);
            }
        }
        std::cout << ".";
    });

    std::cout << std::endl;
    //  OutAverageStream(iIteration);
}

// prepare parameters to be printed out
void OutResults::LoadParameters() {
    std::vector<std::vector<std::vector<InitCellData*>>>& cells = m_pGrid->GetInitCells();

    const Vector3i& vSize = m_pGrid->GetSize();
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
    std::vector<std::vector<std::vector<InitCellData*>>>& cells = m_pGrid->GetInitCells();

    std::string filename;
    const std::string& output_prefix = m_pConfig->getOutputPrefix();

    filename = output_prefix + "out/gas" + to_string(gas) +
               "/" + param_to_str(type) + "/data/" + to_string(index) + ".bin";

    std::ofstream fs(filename.c_str(), std::ios::out | std::ios::binary);

    const Vector3i& vGridSize = m_pConfig->getGridSize();

    // writing additional info
    double tmp = static_cast<double>(vGridSize.x());
    fs.write(reinterpret_cast<const char*>(&tmp), sizeof(double));
    tmp = static_cast<double>(vGridSize.y());
    fs.write(reinterpret_cast<const char*>(&tmp), sizeof(double));

    unsigned int params_num = type == sep::FLOW_PARAM ? 2 : 1;
    std::vector<double> params(params_num);

    const Normalizer* pNormalizer = m_pConfig->getNormalizer();

    for (int y = 0; y < vGridSize.y(); y++) {
        for (int x = 0; x < vGridSize.x(); x++) {
            int z = 0;

            Cell* cell = cells[x][y][z]->m_pCell;
            if (cell && cells[x][y][z]->m_eType == sep::NORMAL_CELL) {

                // from normalized values back to normal
                const double t = pNormalizer->restore(static_cast<const double&>(cell->m_vMacroData[gas].dTemperature), Normalizer::Type::TEMPERATURE);
                const double n = pNormalizer->restore(static_cast<const double&>(cell->m_vMacroData[gas].dDensity), Normalizer::Type::DENSITY);
                const double p = pNormalizer->restore(static_cast<const double&>(cell->m_vMacroData[gas].dPressure), Normalizer::Type::PRESSURE);
                const double stream_x = pNormalizer->restore(static_cast<const double&>(cell->m_vMacroData[gas].Stream.x()), Normalizer::Type::FLOW);
                const double stream_y = pNormalizer->restore(static_cast<const double&>(cell->m_vMacroData[gas].Stream.y()), Normalizer::Type::FLOW);

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
    const GasVector& vGases = m_pConfig->getGases();

    const std::string& output_prefix = m_pConfig->getOutputPrefix();
    std::string as_filename_base = output_prefix + "out/gas";
    for (unsigned int gi = 0; gi < m_pConfig->getGasesNum(); gi++) {
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

            if (m_pConfig->isGPRTGrid()) {
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
    const Vector3i& grid_size = m_pGrid->GetSize();
    double left_average_stream = ComputeAverageColumnStream(1, gas_n, 0, grid_size.y());
    double right_average_stream = ComputeAverageColumnStream(grid_size.x() - 2, gas_n, 0, grid_size.y());

    filestream.write(reinterpret_cast<const char*>(&left_average_stream), sizeof(double));
    filestream.write(reinterpret_cast<const char*>(&right_average_stream), sizeof(double));

    std::cout << left_average_stream << " : " << right_average_stream << std::endl;
}

void OutResults::OutAverageStreamGPRT(std::fstream& filestream, int gas_n) {
    const Vector3i& grid_size = m_pGrid->GetSize();
    HTypeGridConfig* h_type_config = m_pConfig->getHTypeGridConfig();
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
    std::vector<std::vector<std::vector<InitCellData*>>>& m_vCells = m_pGrid->GetInitCells();

    double average_stream = 0.0;
    for (int y = start_y; y < start_y + size_y; y++) {
        average_stream += m_vCells[index_x][y][0]->m_vMacroData[gi].Stream.x();
    }

    average_stream /= size_y;

    return average_stream;
}
