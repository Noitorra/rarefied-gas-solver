#include "main.h"
#include "solver.h"
#include "grid.h"
#include "grid_manager.h"

#include <memory>
// Simple main function to debug some demo functionality

int main(int argc, const char * argv[])
{
  // Just testing some reading/writing functions

  std::shared_ptr<Solver> pSolver(new Solver());
  pSolver->Init(); // linking children and initialization
  
  // Building grid configuration
  pSolver->GetGrid()->BuildWithActiveConfig();

  return 0;
}
