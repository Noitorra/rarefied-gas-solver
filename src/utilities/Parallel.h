#ifndef PARALLEL_H
#define PARALLEL_H

#include <string>

class Parallel {
public:
    static const int COMMAND_MESH             = 100;
    static const int COMMAND_TIMESTEP         = 110;
    static const int COMMAND_SYNC_IDS         = 200;
    static const int COMMAND_SYNC_VALUES      = 210;
    static const int COMMAND_SYNC_HALF_VALUES = 220;
    static const int COMMAND_RESULT_PARAMS    = 300;
    static const int COMMAND_CONFIG           = 120;

private:
    static bool _isUsingMPI;
    static bool _isSingle;

public:
    static void init();

    static void finalize();

    static bool isUsingMPI();

    static bool isMaster();

    static bool isSlave();

    static bool isSingle();

    static int getSize();

    static int getRank();

    static std::string getProcessorName();


    static void send(const std::string& buffer, int dest, int tag);

    static std::string recv(int source, int tag);

    static void abort();
};

#endif // PARALLEL_H