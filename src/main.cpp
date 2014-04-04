#include "main.h"
#include "solver.h"

#include <memory>
// Simple main function to debug some demo functionality

int main(int argc, const char * argv[])
{
  // just testing some reading/writing functions
  // waiting for boost::shared_ptr
  std::shared_ptr<Solver> pSolver(new Solver());

  return 0;
}
