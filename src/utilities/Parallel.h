#ifndef PARALLEL_H
#define PARALLEL_H

#include <string>

class Parallel {
public:
    static const int COMMAND_MESH                   = 100;
    static const int COMMAND_TIMESTEP               = 110;
    static const int COMMAND_CONFIG                 = 120;
    static const int COMMAND_MESSAGE                = 130;
    static const int COMMAND_SYNC_IDS               = 200;
    static const int COMMAND_SYNC_VALUES            = 210;
    static const int COMMAND_SYNC_HALF_VALUES       = 220;
    static const int COMMAND_RESULT_PARAMS          = 300;
    static const int COMMAND_BUFFER_RESULTS         = 310;
    static const int COMMAND_BUFFER_AVERAGE_RESULTS = 320;

private:
    static bool _isUsingMPI;
    static bool _isSingle;
    static int _size;
    static int _rank;
    static std::string _name;

public:
    static void init(int *argc, char ***argv);

    static void finalize();

    static void send(const std::string& buffer, int dest, int tag);

    static std::string recv(int source, int tag);

    static void abort();

    static void barrier();

    static bool isUsingMPI() {
        return _isUsingMPI;
    }

    static bool isMaster() {
        return getRank() == 0;
    }

    static bool isSlave() {
        return getRank() != 0;
    }

    static bool isSingle() {
        return _isSingle;
    }

    static int getSize() {
        return _size;
    }

    static int getRank() {
        return _rank;
    }

    static std::string getName() {
        return _name;
    }

};

#endif // PARALLEL_H