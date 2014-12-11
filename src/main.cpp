#include "main.h"
#include "solver.h"
#include "grid.h"
#include "grid_manager.h"
#include "parallel.h"
#include "out_results.h"

#include <memory>
// Simple main function to debug some demo functionality

int main(int argc, char * argv[])
{
  std::shared_ptr<GridManager> pGridManager(new GridManager());

  pGridManager->Init();
  
  pGridManager->ConfigureGrid();
  
  pGridManager->GetSolver()->Run();



  return 0;
}
