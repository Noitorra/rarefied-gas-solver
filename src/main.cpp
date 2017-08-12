#include "utilities/parallel.h"
#include "grid_new/GridMaker.h"
#include "grid_new/Grid.h"

int main(int argc, char* argv[]) {
    //std::ofstream out("log.txt");
    //std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
    //std::cout.rdbuf(out.rdbuf()); //redirect std::cout to out.txt!

    /*
    Config::getInstance()->Init();

    Solver pSolver;
    pSolver.init();
    pSolver.Run();
     */

    Parallel::init();

    // Print off a hello world message
//    std::cout << "Hello world from processor " << Parallel::getProcessorName()
//              << ", rank " << Parallel::getRank()
//              << " out of " << Parallel::getSize() << " processors."
//              << std::endl;

    // Create config
//    Config::getInstance()->init();

    // Create grid
    Grid* grid = GridMaker::makeGrid({100, 25}, 1);
//    std::cout << *grid << std::endl;

    Parallel::finalize();
}
