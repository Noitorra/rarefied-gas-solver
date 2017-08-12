#include "parallel.h"
#include <mpi.h>
#include <cstring>

bool Parallel::_isUsingMPI = false;

void Parallel::init() {
    MPI::Init();
    _isUsingMPI = true;
}

void Parallel::finalize() {
    MPI::Finalize();
    _isUsingMPI = false;
}

bool Parallel::isMaster() {
    return getRank() == 0;
}

bool Parallel::isSlave() {
    return getRank() != 0;
}

int Parallel::getSize() {
    return MPI::COMM_WORLD.Get_size();
}

int Parallel::getRank() {
    return MPI::COMM_WORLD.Get_rank();
}

char *Parallel::getProcessorName() {
    char* processor_name = new char[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI::Get_processor_name(processor_name, name_len);
    return processor_name;
}

bool Parallel::isUsingMPI() {
    return _isUsingMPI;
}

void Parallel::send(const char* buffer, int dest, int tag) {
    MPI::COMM_WORLD.Send(buffer, (int) std::strlen(buffer), MPI_BYTE, dest, tag);
}

const char* Parallel::receive(int source, int tag) {
    MPI::Status status;
    MPI::COMM_WORLD.Probe(source, tag, status);

    int len = status.Get_count(MPI_BYTE);
    char* buffer = new char[len];

    MPI::COMM_WORLD.Recv(buffer, len, MPI_BYTE, source, tag);

    return buffer;
}

