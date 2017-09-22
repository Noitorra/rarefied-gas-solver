#ifndef PARALLEL_H_
#define PARALLEL_H_

class Parallel {
public:
    static const int COMMAND_GRID = 100;
    static const int COMMAND_SYNC_IDS = 200;
    static const int COMMAND_SYNC_VALUES = 300;
private:
    static bool _isUsingMPI;

public:
    static void init();

    static void finalize();

    static bool isMaster();

    static bool isSlave();

    static int getSize();

    static int getRank();

    static char* getProcessorName();

    static bool isUsingMPI();

    static void send(const char* buffer, int dest, int tag);

    static const char* recv(int source, int tag);
};

#endif // PARALLEL_H_