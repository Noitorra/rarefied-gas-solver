#include "main.h"
#include "solver.h"
#include "grid.h"
#include "grid_manager.h"

#include <memory>
// Simple main function to debug some demo functionality

int main(int argc, const char * argv[])
{
  // just testing some reading/writing functions

  std::shared_ptr<Solver> pSolver(new Solver());
  pSolver->Init(); // linking children and initialization
  
  // building grid configuration
  pSolver->GetGrid()->SaveConfiguration(sep::DIMAN_CONFIG);
  
  // reading config file
  pSolver->GetGrid()->LoadConfiguration(sep::DIMAN_CONFIG);
  return 0;
}
