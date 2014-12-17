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
#include "parallel.h"

Solver::Solver() :
m_pParallel(new Parallel),
m_pImpulse(new Impulse),
m_pGridManager(nullptr),
m_pGrid(nullptr)
{}

void Solver::Init(GridManager* pGridManager) {
  m_pGridManager = pGridManager;
  m_pGrid = pGridManager->GetGrid();

  m_vGas.push_back( std::shared_ptr<Gas>(new Gas(1.0)) );
	//m_vGas.push_back( std::shared_ptr<Gas>(new Gas(0.5)) );
  
  m_pImpulse->Init(pGridManager);
}

void Solver::Run() {
  std::vector<std::shared_ptr<Cell>>& vCellVector = m_pGrid->GetCells();

  PreRun();
  
  if (Config::bUseIntegral) {
    ci::HSPotential potential;
    ci::init(&potential, ci::NO_SYMM);
  }

  // debug
  std::shared_ptr<OutResults> out_results(new OutResults());
  out_results->Init(m_pGrid, m_pGridManager);

  for(int it = 0; it < Config::iMaxIteration; it++) {
    // debug
    out_results->OutAll(it);

    MakeStep(sep::X);
    MakeStep(sep::Y);
    MakeStep(sep::Z);

    if (Config::bUseIntegral) {
      if (m_vGas.size() == 2) {
        MakeIntegral(0, 0, Config::dTimestep);
        MakeIntegral(0, 1, Config::dTimestep * 2);
        MakeIntegral(1, 1, Config::dTimestep);
      }
      else {
        MakeIntegral(0, 0, Config::dTimestep);
      }
    }

    // here we can test data, if needed...
    for( auto& item : vCellVector ) {
      item->testInnerValuesRange();
    }

    std::cout << "Run() : " << it << "/" << Config::iMaxIteration << std::endl;
  }

  // debug
  out_results->OutAll(Config::iMaxIteration);

  std::cout << "Done..." << std::endl;
}

void Solver::PreRun() {
  InitCellType(sep::X);
  InitCellType(sep::Y);
  InitCellType(sep::Z);
  
  m_pGridManager->PrintLinkage(sep::X);
  m_pGridManager->PrintLinkage(sep::Y);
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
  for( auto& item : cellVector ) {
      item->computeHalf(axis);
  }
  
  // Vessels
  const std::vector<std::shared_ptr<VesselGrid>>& vVessels =
  m_pGrid->GetVessels();
  
  for (auto& item : vVessels) {
    item->computeHalf(axis);
  }
  
  // Make value
  for( auto& item : cellVector ) {
      item->computeValue(axis);
  }
  
  // Vessels
  for (auto& item : vVessels) {
    item->computeValue(axis);
  }
}

void Solver::MakeIntegral(unsigned int gi0, unsigned int gi1, double timestep) {
	GasVector& gasv = GetGas();
  ci::Particle particle;
  particle.d = 1.;

  ci::gen(timestep, 50000, m_pImpulse->getResolution() / 2, m_pImpulse->getResolution() / 2,
    m_pImpulse->getXYZ2I(), m_pImpulse->getXYZ2I(),
    m_pImpulse->getMaxImpulse() / (m_pImpulse->getResolution() / 2),
    gasv[gi0]->getMass(), gasv[gi1]->getMass(),
    particle, particle);

  std::vector<std::shared_ptr<Cell>>& cellVector = m_pGrid->GetCells();

  for( auto& item : cellVector ) {
  	item->computeIntegral(gi0, gi1);
  }
  
  // Vessels
  const std::vector<std::shared_ptr<VesselGrid>>& vVessels =
  m_pGrid->GetVessels();
  
  for (auto& item : vVessels) {
    item->computeIntegral(gi0, gi1);
  }
}
