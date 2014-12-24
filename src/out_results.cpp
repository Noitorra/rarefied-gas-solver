#include "out_results.h"
#include "grid.h"
#include "grid_manager.h"
#include "cell.h"
#include "config.h"
#include "vessel_grid.h"
#include "gas.h"
#include "solver.h"
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

std::string param_to_str(sep::MacroParamType param) {
  switch (param) {
    case sep::T_PARAM:
      return "temp";
    case sep::C_PARAM:
      return "conc";
    case sep::P_PARAM:
      return "pressure";
    default:
      return "no_param";
  }
}

void OutResults::Init(Grid* grid, GridManager* grid_manager) {
  grid_ = grid;
  grid_manager_ = grid_manager;


  // check if needed to create directories
  namespace fs = boost::filesystem;

  for (int param = 0; param < (int)sep::LAST_PARAM; param++) {
    for (int gas = 0; gas < Config::iGasesNumber; gas++) {
      std::string dir = Config::sOutputPrefix + "out/gas" + std::to_string(gas) +
              "/" + param_to_str((sep::MacroParamType)param) + "/data/";
      fs::path file_path(dir);

      boost::system::error_code ec;
      fs::remove_all(file_path.parent_path(), ec);
      if (ec)
        throw("cannot remove directory");
      boost::filesystem::create_directories(file_path, ec);
      if (ec)
        throw("cannot create directory");
      fs::path pic_dir(file_path.parent_path().parent_path() / fs::path("pic"));
      boost::filesystem::create_directories(pic_dir, ec);
      if (ec)
        throw("cannot create directory");
    }
  }
}

void OutResults::OutAll(int iteration) {
  if (!grid_ || !grid_manager_) {
    std::cout << "Error: member OutResults is not initialized yet" << std::endl;
    return;
  }
  std::cout << "Out results " << iteration << " ..." << std::endl;

  LoadParameters();
  for (int gas = 0; gas < Config::iGasesNumber; gas++) {
    OutParameter(sep::T_PARAM, gas, iteration);
    OutParameter(sep::C_PARAM, gas, iteration);
    OutParameter(sep::P_PARAM, gas, iteration);
  }
  OutAverageStream(iteration);
}

// prepare parameters to be printed out
void OutResults::LoadParameters() {
  std::vector<std::vector<std::vector<std::shared_ptr<InitCellData>>>>&cells = grid_->GetInitCells();
  
  const Vector3i& vSize = grid_->GetSize();
  for (int x = 0; x < vSize.x(); x++) {
    for (int y = 0; y < vSize.y(); y++) {
      const int z = 0;
      if (cells[x][y][z]->m_eType != sep::NORMAL_CELL)
        continue;
      Cell* cell = cells[x][y][z].get()->m_pCell;
      cell->computeMacroData();
    }
  }
  
  const std::vector<std::shared_ptr<VesselGrid>>& vVessels = grid_->GetVessels();
  
  for (auto& pVessel : vVessels) {
    pVessel->computeMacroData();
  }
}

void OutResults::OutParameter(sep::MacroParamType type, int gas, int index) {
  std::vector<std::vector<std::vector<std::shared_ptr<InitCellData>>>>&cells = grid_->GetInitCells();

  std::string filename;
  const std::string& output_prefix = Config::sOutputPrefix;

  filename = output_prefix + "out/gas" + std::to_string(gas) +
          "/" + param_to_str(type) + "/data/" + std::to_string(index) + ".bin";

  std::ofstream fs(filename.c_str(), std::ios::out | std::ios::binary);

  const Vector3i& grid_size = Config::vGridSize;

  // writing additional info
  double tmp = (double)grid_size.x();
  fs.write(reinterpret_cast<const char*>(&tmp), sizeof(double));
  tmp = (double)grid_size.y();
  fs.write(reinterpret_cast<const char*>(&tmp), sizeof(double));

  for (int y = 0; y < grid_size.y(); y++) {
    for (int x = 0; x < grid_size.x(); x++) {
      int z = 0;
      double param = 0.0;

      Cell* cell = cells[x][y][z]->m_pCell;
      if (cell && cells[x][y][z]->m_eType == sep::NORMAL_CELL) {
        switch (type) {
          case sep::T_PARAM:
            param = cell->m_vMacroData[gas].T;
            break;
          case sep::C_PARAM:
            param = cell->m_vMacroData[gas].C;
            break;
          case sep::P_PARAM:
            param = cell->m_vMacroData[gas].C * cell->m_vMacroData[gas].T;
            break;
          default:
            return;
        }
      }

      fs.write(reinterpret_cast<const char*>(&param), sizeof(double));
    }
  }
  fs.close();
}

void OutResults::OutAverageStream(int iteration) {
  GasVector& gasv = Config::vGas;

  const std::string& output_prefix = Config::sOutputPrefix;
  std::string as_filename_base = output_prefix + "out/gas";
  for (unsigned int gi = 0; gi < gasv.size(); gi++) {
    std::string as_filename = as_filename_base + std::to_string(gi);
    as_filename += "/";
    as_filename += "average_stream.bin";

    std::fstream test;
    test.open("test.bin", std::ios::out | std::ios::binary);
    test.close();

    std::fstream filestream;
    std::ios::openmode openmode;
    if (iteration == 0) {
      openmode = std::ios::out | std::ios::binary;
    } else {
      openmode = std::ios::app | std::ios::binary;
    }
    filestream.open(as_filename, openmode);
    if (filestream.is_open()) {

      switch (Config::eGridGeometryType) {
        case sep::COMB_GRID_GEOMETRY:
          OutAverageStreamComb(filestream, gi);
          break;
        case sep::H_GRID_GEOMTRY:
          OutAverageStreamHType(filestream, gi);
          break;
        default:
          return;
      }
      filestream.close();
    }
    else {
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

void OutResults::OutAverageStreamHType(std::fstream& filestream, int gas_n) {
  const Vector3i& grid_size = grid_->GetSize();
  HTypeGridConfig* h_type_config = Config::pHTypeGridConfig.get();
  int iShiftX = 5;
  int D = h_type_config->D;
  int l = h_type_config->l;
  
  double dLeftUpStream = ComputeAverageColumnStream(iShiftX, gas_n, 1, D - 2);
  double dRightUpStream = ComputeAverageColumnStream(grid_size.x() - 1 - iShiftX, gas_n, 1, D - 2);
  double dLeftDownStream = ComputeAverageColumnStream(iShiftX, gas_n, D + l + 1, D - 2);
  double dRightDownStream = ComputeAverageColumnStream(grid_size.x() - 1 - iShiftX, gas_n, D + l + 1, D - 2);
  
  filestream.write(reinterpret_cast<const char*>(&dLeftUpStream), sizeof(double));
  filestream.write(reinterpret_cast<const char*>(&dRightUpStream), sizeof(double));
  filestream.write(reinterpret_cast<const char*>(&dLeftDownStream), sizeof(double));
  filestream.write(reinterpret_cast<const char*>(&dRightDownStream), sizeof(double));
  
  std::cout << dLeftUpStream << " : " << dRightUpStream << " : " <<
  dLeftDownStream << " : " << dRightDownStream << std::endl;
}

double OutResults::ComputeAverageColumnStream(int index_x, unsigned int gi, int start_y, int size_y) {
  std::vector<std::vector<std::vector<std::shared_ptr<InitCellData>>>>& m_vCells = grid_->GetInitCells();

  double average_stream = 0.0;
  for (int y = start_y; y < start_y + size_y; y++) {
    average_stream += m_vCells[index_x][y][0]->m_vMacroData[gi].Stream.x();
  }

  average_stream /= size_y;
 
  return average_stream;
}