#ifndef BETA_CHAIN_H_
#define BETA_CHAIN_H_

class BetaChain {
public:
    BetaChain(int gasIndex1, int gasIndex2, int gasIndex3, double lambda1, double lambda2);

    unsigned int iGasIndex1;
    unsigned int iGasIndex2;
    unsigned int iGasIndex3;
    double dLambda1;
    double dLambda2;
};

#endif // BETA_CHAIN_H_
