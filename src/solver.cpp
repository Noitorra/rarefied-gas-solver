#include "solver.h"

#include "parallel.h"
#include "solver_info.h"
#include "options.h"
#include "grid.h"
#include "grid_manager.h"
#include "cell.h"
#include "out_results.h"
#include "config.h"
#include "impulse.h"
#include "gas.h"

#include "vessel_grid.h"

#include "integral/ci.hpp"

Solver::Solver()
: m_pParallel(new Parallel),
  m_pSolverInfo(new SolverInfo),
  m_pGridManager(new GridManager)
{
	// TODO Auto-generated constructor stub
}

Solver::~Solver() {
	// TODO Auto-generated destructor stub
}

Grid* Solver::GetGrid() const { return m_pGridManager->GetGrid(); }

void Solver::Init() {
	// We must OBEY order:
	// first info (impulse, etc)
	// second grid....
  m_pSolverInfo->setSolver(this);
  m_pSolverInfo->Init();

  // Init grid manager
  m_pGridManager->SetParent(this);
  m_pGridManager->Init();
  m_pGrid = m_pGridManager->GetGrid();
  
  m_pGridManager->BuildGrid();

  initCellType(sep::X);
  initCellType(sep::Y);
  initCellType(sep::Z);

  //LeftVesselGrid lvg;
  //lvg.setGridManager(m_pGridManager.get());

  //lvg.getVesselGridInfo()->dStartConcentration = 1.0;
  //lvg.getVesselGridInfo()->dStartTemperature = 1.0;
  //lvg.getVesselGridInfo()->iAdditionalLenght = 2;
  //lvg.getVesselGridInfo()->iNy = 6;
  //lvg.getVesselGridInfo()->iNz = 1;
  //lvg.getVesselGridInfo()->vAreastep = Vector3d(0.1, 0.1, 0.1);

  //lvg.SetVesselGridType(VesselGrid::VGT_NORMAL); 
  //lvg.CreateAndLinkVessel(); // not working, waiting m_pGrid .... no pCell->Init();
  //lvg.PrintLinkage(sep::X);

//  m_pGrid->GetGridManager()->Print(sep::X);
//  m_pGrid->GetGridManager()->Print(sep::Y);
  // m_pGrid->GetGridManager()->Print(sep::Z);
}

void Solver::Run() {
	std::vector<std::shared_ptr<Cell>>& cellVector = m_pGrid->getCellVector();

  // TODO: Add some flag, which determines integral usage
  // if we want to use integral ...
  ci::HSPotential potential;
  ci::init(&potential, ci::NO_SYMM);
  
  // Save initial state
  m_pGridManager->GetOutResults()->OutAll(0);

	for(int iteration = 0;iteration<GetConfig()->GetMaxIteration();iteration++) {
    
		makeStep(sep::X);
		makeStep(sep::Y);
		makeStep(sep::Z);

    if (GetConfig()->GetUseIntegral()) {
      if (m_pSolverInfo->getGasVector().size() == 2) {
        makeIntegral(0, 0, GetConfig()->GetTimeStep());
        makeIntegral(0, 1, GetConfig()->GetTimeStep() * 2);
        makeIntegral(1, 1, GetConfig()->GetTimeStep());
      }
      else {
        makeIntegral(0, 0, GetConfig()->GetTimeStep());
      }
    }

		// here we can test data, if needed...
		for( auto& item : cellVector ) {
			item->testInnerValuesRange();
		}
    
    // Output data
    m_pGridManager->GetOutResults()->OutAll(iteration + 1);
    std::cout << "Run() : " << iteration << "/" << GetConfig()->GetMaxIteration() << std::endl;
	}
}

void Solver::initCellType(sep::Axis axis) {
	std::vector<std::shared_ptr<Cell>>& cellVector = m_pGrid->getCellVector();
	// make type
	for( auto& item : cellVector ) {
		item->computeType(axis);
	}
}

void Solver::makeStep(sep::Axis axis) {
	std::vector<std::shared_ptr<Cell>>& cellVector = m_pGrid->getCellVector();
	// make half
	for( auto& item : cellVector ) {
		item->computeHalf(axis);
	}
	// make value
	for( auto& item : cellVector ) {
		item->computeValue(axis);
	}
}

Config* Solver::GetConfig() const {
  return m_pSolverInfo->getOptions()->GetConfig();
}

void Solver::makeIntegral(unsigned int gi0, unsigned int gi1, double timestep) {
  GasVector& gasv = getSolverInfo()->getGasVector();
  Impulse* impulse = getSolverInfo()->getImpulse();

  ci::Particle particle;
  particle.d = 1.;

  ci::gen(timestep, 50000, impulse->getResolution() / 2, impulse->getResolution() / 2,
    impulse->getXYZ2I(), impulse->getXYZ2I(),
    impulse->getMaxImpulse() / (impulse->getResolution() / 2),
    gasv[gi0]->getMass(), gasv[gi1]->getMass(),
    particle, particle);

  std::vector<std::shared_ptr<Cell>>& cellVector = m_pGrid->getCellVector();

  for( auto& item : cellVector ) {
  	item->computeIntegral(gi0, gi1);
  }
}
