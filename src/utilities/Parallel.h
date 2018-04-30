#ifndef PARALLEL_H_
#define PARALLEL_H_

#include <string>

class Parallel {
public:
    static const int COMMAND_GRID             = 100;
    static const int COMMAND_GRID_TIMESTEP    = 110;
    static const int COMMAND_SYNC_IDS         = 200;
    static const int COMMAND_SYNC_VALUES      = 210;
    static const int COMMAND_SYNC_HALF_VALUES = 220;
    static const int COMMAND_RESULT_PARAMS    = 300;
private:
    static bool _isUsingMPI;

public:
    static void init();

    static void finalize();

    static bool isMaster();

    static bool isSlave();

    static int getSize();

    static int getRank();

    static std::string getProcessorName();

    static bool isUsingMPI();

    static void send(const std::string& buffer, int dest, int tag);

    static std::string recv(int source, int tag);

    static void abort();
};

#endif // PARALLEL_H_