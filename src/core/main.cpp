#include <iostream>
#include "utilities/config.h"
#include "utilities/parallel.h"
#include "grid/GridMaker.h"
#include "core/Solver.h"

int main(int argc, char* argv[]) {
    //std::ofstream out("log.txt");
    //std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
    //std::cout.rdbuf(out.rdbuf()); //redirect std::cout to out.txt!

    Parallel::init();

    // Print off a hello world message
    std::cout << "Hello world from processor " << Parallel::getProcessorName() << ", rank " << Parallel::getRank()
              << " out of " << Parallel::getSize() << " processors." << std::endl;

    // Create config
    Config::getInstance()->init();

    // Create grid
    Grid<CellData>* grid = GridMaker::makeGrid({42, 22});

    Solver solver{};
    solver.init(grid);
    solver.run();

    Parallel::finalize();
}
