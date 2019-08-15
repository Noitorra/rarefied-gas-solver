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
    double _meshUnits;

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

    bool _isImplicitScheme;

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

    const std::string& getMeshFilename() const {
        return _meshFilename;
    }

    double getMeshUnits() const {
        return _meshUnits;
    }

    const std::string& getOutputFolder() const {
        return _outputFolder;
    }

    unsigned int getMaxIterations() const {
        return _maxIterations;
    }

    unsigned int getOutEachIteration() const {
        return _outEachIteration;
    }

    bool isUsingIntegral() const {
        return _isUsingIntegral;
    }

    bool isUsingBetaDecay() const {
        return _isUsingBetaDecay;
    }

    const std::vector<Gas>& getGases() const {
        return _gases;
    }

    const std::vector<BetaChain>& getBetaChains() const {
        return _betaChains;
    }

    const std::vector<InitialParameters>& getInitialParameters() const {
        return _initialParameters;
    }

    const std::vector<BoundaryParameters>& getBoundaryParameters() const {
        return _boundaryParameters;
    }

    Normalizer* getNormalizer() const {
        return _normalizer.get();
    }

    ImpulseSphere* getImpulseSphere() const {
        return _impulseSphere.get();
    }

    void setTimestep(double timestep) {
        _timestep = timestep;
    }

    double getTimestep() const {
        return _timestep;
    }

    bool isImplicitScheme() const {
        return _isImplicitScheme;
    }

    friend std::ostream& operator<<(std::ostream& os, const Config& config);

private:
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & _meshFilename;
        ar & _meshUnits;
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

        ar & _isImplicitScheme;
    }

};

#endif // RGS_CONFIG_H
