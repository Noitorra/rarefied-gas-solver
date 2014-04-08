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

#include "integral/ci.hpp"

Solver::Solver()
: m_pParallel(new Parallel),
  m_pSolverInfo(new SolverInfo),
  m_pGrid(new Grid)
{
	// TODO Auto-generated constructor stub
}

Solver::~Solver() {
	// TODO Auto-generated destructor stub
}

void Solver::Init() {
	// We must OBEY order:
	// first info (impulse, etc)
	// second grid....
  m_pSolverInfo->setSolver(this);
  m_pSolverInfo->Init();

  // Init grid
  m_pGrid->SetParent(this);
  m_pGrid->Init();
  
  // Building grid configuration
  m_pGrid->BuildWithActiveConfig();

  initCellType(sep::X);
  initCellType(sep::Y);
  initCellType(sep::Z);

  m_pGrid->GetGridManager()->Print(sep::X);
  m_pGrid->GetGridManager()->Print(sep::Y);
  // m_pGrid->GetGridManager()->Print(sep::Z);
}

void Solver::Run() {
	std::vector<std::shared_ptr<Cell>>& cellVector = m_pGrid->getCellVector();

  // Save initial state
  m_pGrid->GetOutResults()->OutAll(0);

	for(int iteration = 0;iteration<GetConfig()->GetMaxIteration();iteration++) {
    
		makeStep(sep::X);
//		makeStep(sep::Y);
//		makeStep(sep::Z);

    // TODO: Add some flag, which determines integral usage
    // make integral
    //if (m_pSolverInfo->getGasVector().size() == 2) {
    //  makeIntegral(0, 0, GetConfig()->GetTimeStep());
    //  makeIntegral(0, 1, GetConfig()->GetTimeStep() * 2);
    //  makeIntegral(1, 1, GetConfig()->GetTimeStep());
    //}
    //else {
    //  makeIntegral(0, 0, GetConfig()->GetTimeStep());
    //}


		// here we can test data, if needed...
		for( auto& item : cellVector ) {
			item->testInnerValuesRange();
		}
    
    // Output data
    m_pGrid->GetOutResults()->OutAll(iteration + 1);
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

void Solver::initIntegral() {
  ci::HSPotential potential;
  ci::init(&potential, ci::NO_SYMM);
}

void Solver::makeIntegral(unsigned int gi0, unsigned int gi1, double timestep) {
  GasVector& gasv = getSolverInfo()->getGasVector();
  Impulse* impulse = getSolverInfo()->getImpulse();

  ci::Particle particle;
  particle.d = 1.;

  ci::gen(timestep, 50000, impulse->getResolution() / 2, impulse->getResolution() / 2,
    impulse->getXYZ2I(), impulse->getXYZ2I(),
    impulse->getMaxImpulse() / (impulse->getResolution() / 2),
    gasv[0]->getMass(), gasv[0]->getMass(),
    particle, particle);

  std::vector<std::shared_ptr<Cell>>& cellVector = m_pGrid->getCellVector();

  for( auto& item : cellVector ) {
  	item->computeIntegral(gi0, gi1);
  }
}
