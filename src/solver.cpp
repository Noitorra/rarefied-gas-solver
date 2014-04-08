#include "solver.h"

#include "parallel.h"
#include "solver_info.h"
#include "options.h"
#include "grid.h"
#include "grid_manager.h"
#include "cell.h"
#include "out_results.h"
#include "config.h"

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
  m_pGrid->GetGridManager()->Print(sep::Z);
}

void Solver::Run() {
	std::vector<std::shared_ptr<Cell>>& cellVector = m_pGrid->getCellVector();


	for(int iteration = 0;iteration<GetConfig()->GetMaxIteration();iteration++) {
		makeStep(sep::X);
//		makeStep(sep::Y);
//		makeStep(sep::Z);

		// here we can test data, if needed...
		for( auto& item : cellVector ) {
			item->testInnerValuesRange();
		}

    for (auto& item : cellVector) {
      item->computeMacroData();
    }
	}
  // Saving data
  m_pGrid->GetOutResults()->OutAll();
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
//	// make integral
//	// TODO: add integral count...
//	for( auto& item : cellVector ) {
//		item->computeIntegral(axis);
//	}
}

Config* Solver::GetConfig() const {
  return m_pSolverInfo->getOptions()->GetConfig();
}
