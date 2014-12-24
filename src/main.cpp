#include "main.h"
#include "config.h"
#include "solver.h"
#include "grid_manager.h"
#include "out_results.h"


int main(int argc, char * argv[]) {
  // This is very important. DO NOT COMMENT OR DELETE.
  Config::Init();

  std::shared_ptr<GridManager> pGridManager(new GridManager());

  pGridManager->Init();
  
  pGridManager->ConfigureGrid();
  
  pGridManager->GetSolver()->Run();

  return 0;
}
