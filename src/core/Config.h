#ifndef RGS_CONFIG_H
#define RGS_CONFIG_H

#include "utilities/Types.h"
#include "utilities/Normalizer.h"
#include "parameters/Gas.h"
#include "parameters/BetaChain.h"
#include "parameters/InitialParameters.h"
#include "parameters/BoundaryParameters.h"
#include "parameters/ImpulseSphere.h"

#include <vector>
#include <string>
#include <ostream>
#include <memory>

#include <boost/serialization/string.hpp>
#include <boost/serialization/shared_ptr.hpp>

class Config {
    friend class boost::serialization::access;

private:
    std::string _meshFilename;

    std::string _outputFolder;

    unsigned int _maxIterations;
    unsigned int _outEachIteration;

    bool _isUsingIntegral;
    bool _isUsingBetaDecay;

    std::vector<Gas> _gases;
    std::vector<BetaChain> _betaChains;

    std::vector<InitialParameters> _initialParameters;
    std::vector<BoundaryParameters> _boundaryParameters;

    std::shared_ptr<Normalizer> _normalizer;
    std::shared_ptr<ImpulseSphere> _impulseSphere;

    double _timestep;

    static Config* _instance;

public:
    Config() = default;

    static Config* getInstance() {
        if (_instance == nullptr) {
            _instance = new Config();
        }
        return _instance;
    }

    static void setInstance(Config* config) {
        _instance = config;
    }

    void init();

    void load(const std::string& filename);

    const std::string& getMeshFilename() const;

    const std::string& getOutputFolder() const;

    unsigned int getMaxIterations() const;

    unsigned int getOutEachIteration() const;

    bool isUsingIntegral() const;

    bool isUsingBetaDecay() const;

    const std::vector<Gas>& getGases() const;

    const std::vector<BetaChain>& getBetaChains() const;

    const std::vector<InitialParameters>& getInitialParameters() const;

    const std::vector<BoundaryParameters>& getBoundaryParameters() const;

    Normalizer* getNormalizer() const;

    ImpulseSphere* getImpulseSphere() const;

    void setTimestep(double timestep);

    double getTimestep() const;

    friend std::ostream& operator<<(std::ostream& os, const Config& config);

private:
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & _meshFilename;
        ar & _outputFolder;

        ar & _maxIterations;
        ar & _outEachIteration;

        ar & _isUsingIntegral;
        ar & _isUsingBetaDecay;

        ar & _gases;
        ar & _betaChains;

        ar & _initialParameters;
        ar & _boundaryParameters;

        ar & _normalizer;
        ar & _impulseSphere;
    }

};

#endif // RGS_CONFIG_H
