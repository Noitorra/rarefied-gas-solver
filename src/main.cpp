#include "main.h"
#include "config.h"
#include "solver.h"
#include "grid/grid_manager.h"


int main(int argc, char * argv[]) {
  //std::ofstream out("log.txt");
  //std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
  //std::cout.rdbuf(out.rdbuf()); //redirect std::cout to out.txt!

  Config::Init();

  GridManager gridManager;
  gridManager.Init();
  gridManager.ConfigureGrid();
  gridManager.GetSolver()->Run();

  return 0;
}
