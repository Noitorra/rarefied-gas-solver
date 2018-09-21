#include "Parallel.h"

#include <mpi.h>
#include <cstring>

bool Parallel::_isUsingMPI = false;
bool Parallel::_isSingle = true;

void Parallel::init(int *argc, char ***argv) {
    MPI_Init(argc, argv);
    _isUsingMPI = true;
    _isSingle = getSize() == 1;
}

void Parallel::finalize() {
    MPI_Finalize();
    _isUsingMPI = false;
    _isSingle = true;
}

bool Parallel::isUsingMPI() {
    return _isUsingMPI;
}

bool Parallel::isMaster() {
    return getRank() == 0;
}

bool Parallel::isSlave() {
    return getRank() != 0;
}

bool Parallel::isSingle() {
    return _isSingle;
}

int Parallel::getSize() {
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    return size;
}

int Parallel::getRank() {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    return rank;
}

std::string Parallel::getProcessorName() {
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);
    return std::string(processor_name, static_cast<unsigned long>(name_len));
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
