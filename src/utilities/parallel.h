#ifndef PARALLEL_H_
#define PARALLEL_H_

class Parallel {
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

    static const char* receive(int source, int tag);
};

#endif // PARALLEL_H_