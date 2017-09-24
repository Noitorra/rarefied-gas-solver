#include "Parallel.h"
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

std::string Parallel::getProcessorName() {
    auto* processor_name = new char[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI::Get_processor_name(processor_name, name_len);
    return processor_name;
}

bool Parallel::isUsingMPI() {
    return _isUsingMPI;
}

void Parallel::send(const std::string& buffer, int dest, int tag) {
//    std::cout << "Send from " << getRank() << " to " << dest
//              << " buffer = [" << buffer.size() << "b][" << "..." << "]" << std::endl;

    MPI::COMM_WORLD.Send(buffer.c_str(), static_cast<int>(buffer.size()), MPI_BYTE, dest, tag);
}

std::string Parallel::recv(int source, int tag) {
    MPI::Status status;
    MPI::COMM_WORLD.Probe(source, tag, status);

    int len = status.Get_count(MPI_BYTE);

    char rawBuffer[len];

    MPI::COMM_WORLD.Recv(rawBuffer, len, MPI_BYTE, source, tag);

    std::string buffer{rawBuffer, static_cast<unsigned long>(len)};

//    std::cout << "Recv from " << source << " to " << getRank()
//              << " buffer = [" << buffer.size() << "b][" << "..." << "]" << std::endl;

    return buffer;
}

