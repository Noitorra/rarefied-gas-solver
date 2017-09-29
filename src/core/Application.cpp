#include <iostream>
#include <thread>
#include "utilities/Config.h"
#include "utilities/Parallel.h"
#include "grid/GridMaker.h"
#include "Solver.h"

int main(int argc, char* argv[]) {
    //std::ofstream out("log.txt");
    //std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
    //std::cout.rdbuf(out.rdbuf()); //redirect std::cout to out.txt!

    Parallel::init();

    // Print off a hello world message
    std::cout << "Hello world from processor " << Parallel::getProcessorName()
              << ", rank " << Parallel::getRank()
              << " out of " << Parallel::getSize() << " processors." << std::endl;

    // Create config
    Config::getInstance()->init();
    if (Parallel::isMaster() == true) {
        std::cout << "Config:" << std::endl << *Config::getInstance() << std::endl;
    }

    Solver solver{};
    solver.init();
    solver.run();

    Parallel::finalize();
}
