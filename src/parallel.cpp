/*
 * parallel.cpp
 *
 *  Created on: 04 апр. 2014 г.
 *      Author: kisame
 */

#include "parallel.h"
#include <mpi.h>

Parallel::Parallel() {
	m_dStartTime = 0.0;
	m_dEndTime = 0.0;

	m_iRank = 0;
	m_iSize = 0;
}

Parallel::~Parallel() {}

/* public */
void Parallel::InitMPI(int & argc, char **& argv) {
	MPI::Init(argc, argv);
	m_dStartTime = MPI::Wtime();
	m_iSize = MPI::COMM_WORLD.Get_size();
	m_iRank = MPI::COMM_WORLD.Get_rank();
	construct_msg();
}

void Parallel::FinalizeMPI() {
	// waiting until all nodes execute
	m_dEndTime = MPI::Wtime();
	print_exec_time(m_dEndTime - m_dStartTime);
	MPI::COMM_WORLD.Barrier();
	MPI::Finalize();
}

/* private */
void Parallel::print_exec_time(double exec_time) {
	std::cout << m_sMsg << "Exectime = " << exec_time << "s" << std::endl;
}

void Parallel::construct_msg() {
	m_sMsg = "Process [" + std::to_string(m_iRank) + ":" + std::to_string(m_iSize) + "] ";
}
