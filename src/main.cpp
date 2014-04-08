#include "main.h"
#include "solver.h"
#include "grid.h"
#include "grid_manager.h"
#include "parallel.h"

#include <mpi.h>

#include <memory>
// Simple main function to debug some demo functionality

int main(int argc, char * argv[])
{
  // Just testing some reading/writing functions
  std::shared_ptr<Solver> pSolver(new Solver());
  // FIRST: we need to start mpi process...
  pSolver->getParallel()->InitMPI(argc, argv);

  pSolver->Init(); // Linking children and initialization
  
  // Building grid configuration
  pSolver->GetGrid()->BuildWithActiveConfig();



  // LAST: here we apparently stop all nodes....
  pSolver->getParallel()->FinalizeMPI();
  return 0;
}
