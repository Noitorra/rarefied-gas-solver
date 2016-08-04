#include "solver.h"

#include "grid/grid.h"
#include "grid/grid_manager.h"
#include "grid/cell.h"
#include "out_results.h"
#include "config.h"
#include "parameters/impulse.h"
#include "parameters/gas.h"
#include "parameters/beta_chain.h"
#include "integral/ci.hpp"

Solver::Solver() :
	m_pImpulse(new Impulse),
	m_pGridManager(new GridManager),
	m_pGrid(nullptr),
	m_pOutResults(new OutResults) {}

void Solver::Init() {
	m_pGridManager->ConfigureGrid();
	m_pGridManager->Init(this);
	m_pGrid = m_pGridManager->GetGrid();
	m_pImpulse->Init();
	m_pOutResults->Init(m_pGrid, m_pGridManager);
}

void Solver::Run() {
	// Compute cell type for each axis
	InitCellType(sep::X);
	InitCellType(sep::Y);
	InitCellType(sep::Z);

	//  m_pGridManager->PrintLinkage(sep::X);
	//  m_pGridManager->PrintLinkage(sep::Y);

	if (Config::m_bUseIntegral) {
		ci::Potential* potential = new ci::HSPotential;
		ci::init(potential, ci::NO_SYMM);
	}

	auto startTimestamp = std::chrono::steady_clock::now();
	auto timestamp = startTimestamp;

	for (int it = 0; it < Config::m_iMaxIteration; it++) {
		m_pOutResults->OutAll(it);

		MakeStep(sep::X);
		MakeStep(sep::Y);
		MakeStep(sep::Z);

		if (Config::m_bUseIntegral) {
			if (Config::m_iGasesNum == 1) {
				MakeIntegral(0, 0, Config::m_dTimestep);
			} else if (Config::m_iGasesNum == 2) {
				MakeIntegral(0, 0, Config::m_dTimestep);
				MakeIntegral(0, 1, Config::m_dTimestep);
			} else if (Config::m_iGasesNum >= 3) {
				MakeIntegral(0, 0, Config::m_dTimestep);
				MakeIntegral(0, 1, Config::m_dTimestep);
				MakeIntegral(0, 2, Config::m_dTimestep);
			}
		}

		if (Config::m_bUseBetaChain) {
			for (int i = 0; i < Config::m_iBetaChainsNum; i++) {
				auto& item = Config::m_vBetaChains[i];
				MakeBetaDecay(item->iGasIndex1, item->iGasIndex2, item->dLambda1);
				MakeBetaDecay(item->iGasIndex2, item->iGasIndex3, item->dLambda2);
			}
		}

		CheckCells();

		auto now = std::chrono::steady_clock::now();
		auto wholeTime = std::chrono::duration_cast<std::chrono::seconds>(now - startTimestamp).count();
		auto iterationTime = std::chrono::duration_cast<std::chrono::milliseconds>(now - timestamp).count();

		std::cout << "Run() : " << it << "/" << Config::m_iMaxIteration << std::endl;
		std::cout << "Iteration time: " << iterationTime << " ms" << std::endl;
		std::cout << "Inner time: " << it * Config::tau_normalize * Config::m_dTimestep << " s" << std::endl;
		std::cout << "Real time: " << wholeTime << " s" << std::endl;
		std::cout << "Remaining time: ";
		if (it == 0) {
			std::cout << "Unknown";
		} else {
			std::cout << wholeTime * (Config::m_iMaxIteration - it) / it / 60 << " m";
		}
		std::cout << std::endl;
		std::cout << std::endl;
	}

	m_pOutResults->OutAll(Config::m_iMaxIteration);

	std::cout << "Done" << std::endl;
}

void Solver::InitCellType(sep::Axis axis) {
	std::vector<Cell*>& vCellVector = m_pGrid->GetCells();
	// make type
	for (auto& item : vCellVector) {
		item->computeType(axis);
	}
}

void Solver::MakeStep(sep::Axis axis) {
	std::vector<Cell*>& cellVector = m_pGrid->GetCells();

	// Make half
	tbb::parallel_for_each(cellVector.begin(), cellVector.end(), [&](Cell* item) { item->computeHalf(axis); });

	// Make value
	tbb::parallel_for_each(cellVector.begin(), cellVector.end(), [&](Cell* item) { item->computeValue(axis); });
}

void Solver::MakeIntegral(unsigned int gi0, unsigned int gi1, double timestep) {
	GasVector& gasv = Config::m_vGases;
	ci::Particle particle;
	particle.d = 1.;

	ci::gen(timestep, 50000, m_pImpulse->getResolution() / 2, m_pImpulse->getResolution() / 2,
	        m_pImpulse->getXYZ2I(), m_pImpulse->getXYZ2I(),
	        m_pImpulse->getMaxImpulse() / (m_pImpulse->getResolution() / 2),
	        gasv[gi0]->getMass(), gasv[gi1]->getMass(),
	        particle, particle);

	std::vector<Cell*>& cellVector = m_pGrid->GetCells();
	tbb::parallel_for_each(cellVector.begin(), cellVector.end(), [&](Cell* item) { item->computeIntegral(gi0, gi1); });
}

void Solver::MakeBetaDecay(unsigned int gi0, unsigned int gi1, double lambda) {
	std::vector<Cell*>& vCellVector = m_pGrid->GetCells();
	tbb::parallel_for_each(vCellVector.begin(), vCellVector.end(), [&](Cell* item) { item->computeBetaDecay(gi0, gi1, lambda); });
}

void Solver::CheckCells() {
	std::vector<Cell*>& vCellVector = m_pGrid->GetCells();
	tbb::parallel_for_each(vCellVector.begin(), vCellVector.end(), [&](Cell* item) { item->checkInnerValuesRange(); });
}
