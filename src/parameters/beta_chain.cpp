#include "beta_chain.h"

BetaChain::BetaChain(int gasIndex1, int gasIndex2, int gasIndex3, double lambda1, double lambda2) {
  iGasIndex1 = gasIndex1;
  iGasIndex2 = gasIndex2;
  iGasIndex3 = gasIndex3;

  dLambda1 = lambda1;
  dLambda2 = lambda2;
}