#ifndef PARALLEL_H_
#define PARALLEL_H_

#include <string>

class Parallel {
	double m_dStartTime;
	double m_dEndTime;

	int m_iRank;
	int m_iSize;

	std::string m_sMsg;
public:
	Parallel();

	void InitMPI(int & argc, char **& argv);
	void FinalizeMPI();
private:
	void print_exec_time(double exec_time);
	void construct_msg();
};

#endif /* PARALLEL_H_ */
