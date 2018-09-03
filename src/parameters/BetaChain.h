#ifndef BETA_CHAIN_H
#define BETA_CHAIN_H

#include <ostream>

class BetaChain {
private:
    unsigned int _gasIndex1;
    unsigned int _gasIndex2;
    unsigned int _gasIndex3;
    double _lambda1;
    double _lambda2;

public:
    BetaChain(unsigned int gasIndex1, unsigned int gasIndex2, unsigned int gasIndex3, double lambda1, double lambda2) {
        _gasIndex1 = gasIndex1;
        _gasIndex2 = gasIndex2;
        _gasIndex3 = gasIndex3;

        _lambda1 = lambda1;
        _lambda2 = lambda2;
    }

    friend std::ostream& operator<<(std::ostream& os, const BetaChain& chain) {
        os << "gasIndex1: " << chain._gasIndex1
           << " gasIndex2: " << chain._gasIndex2
           << " gasIndex3: " << chain._gasIndex3
           << " lambda1: " << chain._lambda1
           << " lambda2: " << chain._lambda2;
        return os;
    }
};

#endif // BETA_CHAIN_H
