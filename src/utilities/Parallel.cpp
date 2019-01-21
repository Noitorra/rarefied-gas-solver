#include "Parallel.h"

#include <mpi.h>
#include <cstring>

bool Parallel::_isUsingMPI = false;
bool Parallel::_isSingle = true;
int Parallel::_size = 1;
int Parallel::_rank = 0;
std::string Parallel::_name{};

void Parallel::init(int *argc, char ***argv) {
    MPI_Init(argc, argv);
    _isUsingMPI = true;

    MPI_Comm_size(MPI_COMM_WORLD, &_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &_rank);

    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);
    _name = std::string(processor_name, static_cast<unsigned long>(name_len));

    _isSingle = _size == 1;
}

void Parallel::finalize() {
    _isUsingMPI = false;
    _isSingle = true;
    MPI_Finalize();
}

void Parallel::send(const std::string& buffer, int dest, int tag) {
    MPI_Send(buffer.c_str(), static_cast<int>(buffer.size()), MPI_BYTE, dest, tag, MPI_COMM_WORLD);
}

std::string Parallel::recv(int source, int tag) {
    MPI_Status status;
    MPI_Probe(source, tag, MPI_COMM_WORLD, &status);

    int len;
    MPI_Get_count(&status, MPI_BYTE, &len);

    char* rawBuffer = new char[len];

    MPI_Recv(rawBuffer, len, MPI_BYTE, source, tag, MPI_COMM_WORLD, &status);

    std::string buffer{rawBuffer, static_cast<unsigned long>(len)};

    delete[] rawBuffer;

    return buffer;
}

void Parallel::abort() {
    MPI_Abort(MPI_COMM_WORLD, 1);
}

void Parallel::barrier() {
    MPI_Barrier(MPI_COMM_WORLD);
}
