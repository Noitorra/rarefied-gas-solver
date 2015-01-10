#include "solver.h"

#include "parallel.h"
#include "grid.h"
#include "grid_manager.h"
#include "cell.h"
#include "out_results.h"
#include "config.h"
#include "impulse.h"
#include "gas.h"
#include "vessel_grid.h"
#include "integral/ci.hpp"
#include "timer.h"

Solver::Solver() :
m_pParallel(new Parallel),
m_pImpulse(new Impulse),
m_pGridManager(nullptr),
m_pGrid(nullptr)
{}

void Solver::Init(GridManager* pGridManager) {
  m_pGridManager = pGridManager;
  m_pGrid = pGridManager->GetGrid();
  
  m_pImpulse->Init(pGridManager);
}

void Solver::Run() {
  std::vector<std::shared_ptr<Cell>>& vCellVector = m_pGrid->GetCells();

  PreRun();
  
  if (Config::bUseIntegral) {
    ci::HSPotential potential;
    ci::init(&potential, ci::NO_SYMM);
  }

  std::shared_ptr<OutResults> out_results(new OutResults());
  out_results->Init(m_pGrid, m_pGridManager);

  timer_.restart();
  for(int it = 0; it < Config::iMaxIteration; it++) {

    Timer iter_timer;
    out_results->OutAll(it);

    MakeStep(sep::X);
    MakeStep(sep::Y);
    MakeStep(sep::Z);

    if (Config::bUseIntegral) {
      if (Config::iGasesNumber == 3) {
        MakeIntegral(0, 0, Config::dTimestep);
        MakeIntegral(0, 1, Config::dTimestep);
        MakeIntegral(0, 2, Config::dTimestep);
      } else if (Config::iGasesNumber == 2) {
        MakeIntegral(0, 0, Config::dTimestep);
        MakeIntegral(0, 1, 2 * Config::dTimestep);
        MakeIntegral(1, 1, Config::dTimestep);
      } else {
        MakeIntegral(0, 0, Config::dTimestep);
      }
    }

    // here we can test data, if needed...
    tbb::parallel_for_each(vCellVector.begin(), vCellVector.end(), [&](const std::shared_ptr<Cell>& item) {
      item->testInnerValuesRange();
    });

    std::cout << "Run() : " << it << "/" << Config::iMaxIteration << std::endl;
    PrintElapsedTime(it, iter_timer);
    std::cout << std::endl;
  }

  out_results->OutAll(Config::iMaxIteration);

  std::cout << "Done..." << std::endl;
}

void Solver::PreRun() {
  InitCellType(sep::X);
  InitCellType(sep::Y);
  InitCellType(sep::Z);
  
//  m_pGridManager->PrintLinkage(sep::X);
//  m_pGridManager->PrintLinkage(sep::Y);
}

void Solver::InitCellType(sep::Axis axis) {
	std::vector<std::shared_ptr<Cell>>& vCellVector = m_pGrid->GetCells();
	// make type
	for( auto& item : vCellVector ) {
		item->computeType(axis);
	}
  
  // Vessels
  const std::vector<std::shared_ptr<VesselGrid>>& vVessels =
  m_pGrid->GetVessels();
  
  for (auto& item : vVessels) {
    item->computeType(axis);
  }
}

void Solver::MakeStep(sep::Axis axis) {
  std::vector<std::shared_ptr<Cell>>& cellVector = m_pGrid->GetCells();

  // Make half
  tbb::parallel_for_each(cellVector.begin(), cellVector.end(), [&](const std::shared_ptr<Cell>& item) {
      item->computeHalf(axis);
  });

  // Vessels
  const std::vector<std::shared_ptr<VesselGrid>>& vVessels =
  m_pGrid->GetVessels();

  for (auto& item : vVessels) {
    item->computeHalf(axis);
  }

  // Make value
  tbb::parallel_for_each(cellVector.begin(), cellVector.end(), [&](const std::shared_ptr<Cell>& item) {
      item->computeValue(axis);
  });

  // Vessels
  for (auto& item : vVessels) {
    item->computeValue(axis);
  }
}

void Solver::MakeIntegral(unsigned int gi0, unsigned int gi1, double timestep) {
  GasVector& gasv = Config::vGas;
  ci::Particle particle;
  particle.d = 1.;

  ci::gen(timestep, 50000, m_pImpulse->getResolution() / 2, m_pImpulse->getResolution() / 2,
    m_pImpulse->getXYZ2I(), m_pImpulse->getXYZ2I(),
    m_pImpulse->getMaxImpulse() / (m_pImpulse->getResolution() / 2),
    gasv[gi0]->getMass(), gasv[gi1]->getMass(),
    particle, particle);

  std::vector<std::shared_ptr<Cell>>& cellVector = m_pGrid->GetCells();

  tbb::parallel_for_each(cellVector.begin(), cellVector.end(), [&](const std::shared_ptr<Cell>& item) {
    item->computeIntegral(gi0, gi1);
  });
  
  // Vessels
  const std::vector<std::shared_ptr<VesselGrid>>& vVessels =
  m_pGrid->GetVessels();
  
  for (auto& item : vVessels) {
    item->computeIntegral(gi0, gi1);
  }
}

void Solver::PrintElapsedTime(int it, Timer& iter_timer) {
  std::cout << "Compiuted time in task: " << it * Config::tau_normalize * Config::dTimestep << " s" << std::endl;
  std::cout << "Iteration time: " << iter_timer.elapsed_time() << " ms" << std::endl;
  const double elapsed_time = timer_.elapsed_time() / 1e3;  // s
  std::cout << "Elapsed time: " <<  (int)elapsed_time << " s. " <<
          Config::iMaxIteration << " iteration will be done in " <<
          (int)(elapsed_time / ((double)(it+1) / Config::iMaxIteration) / 60.0)  << " min." << std::endl;
}
