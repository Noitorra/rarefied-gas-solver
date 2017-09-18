#include "beta_chain.h"

BetaChain::BetaChain(int gasIndex1, int gasIndex2, int gasIndex3, double lambda1, double lambda2) {
    iGasIndex1 = gasIndex1;
    iGasIndex2 = gasIndex2;
    iGasIndex3 = gasIndex3;

    dLambda1 = lambda1;
    dLambda2 = lambda2;
}

std::ostream& operator<<(std::ostream& os, const BetaChain& chain) {
    os << "iGasIndex1: " << chain.iGasIndex1
       << " iGasIndex2: " << chain.iGasIndex2
       << " iGasIndex3: " << chain.iGasIndex3
       << " dLambda1: " << chain.dLambda1
       << " dLambda2: " << chain.dLambda2;
    return os;
}
