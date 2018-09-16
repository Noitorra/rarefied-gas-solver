#ifndef RGS_BETA_CHAIN_H
#define RGS_BETA_CHAIN_H

#include <ostream>

#include <boost/serialization/access.hpp>

class BetaChain {
    friend class boost::serialization::access;

private:
    unsigned int _gasIndex1;
    unsigned int _gasIndex2;
    unsigned int _gasIndex3;
    double _lambda1;
    double _lambda2;

public:
    BetaChain() = default;

    BetaChain(unsigned int gasIndex1, unsigned int gasIndex2, unsigned int gasIndex3, double lambda1, double lambda2) {
        _gasIndex1 = gasIndex1;
        _gasIndex2 = gasIndex2;
        _gasIndex3 = gasIndex3;

        _lambda1 = lambda1;
        _lambda2 = lambda2;
    }

    unsigned int getGasIndex1() const {
        return _gasIndex1;
    }

    unsigned int getGasIndex2() const {
        return _gasIndex2;
    }

    unsigned int getGasIndex3() const {
        return _gasIndex3;
    }

    double getLambda1() const {
        return _lambda1;
    }

    double getLambda2() const {
        return _lambda2;
    }

    friend std::ostream& operator<<(std::ostream& os, const BetaChain& chain) {
        os << "gi1 = " << chain._gasIndex1 << " "
           << "gi2 = " << chain._gasIndex2 << " "
           << "gi3 = " << chain._gasIndex3 << " "
           << "lambda1 = " << chain._lambda1 << " | "
           << "lambda2 = " << chain._lambda2;
        return os;
    }

private:
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & _gasIndex1;
        ar & _gasIndex2;
        ar & _gasIndex3;

        ar & _lambda1;
        ar & _lambda2;
    }

};

#endif // RGS_BETA_CHAIN_H
