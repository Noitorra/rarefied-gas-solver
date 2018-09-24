#include "Config.h"

#include <sstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

Config* Config::_instance = nullptr;

void Config::init() {
    if (_normalizer == nullptr) {
        _normalizer.reset(new Normalizer());
    }

    double maxMass = 0.0;
    for (const auto& gas : _gases) {
        maxMass = std::max(maxMass, gas.getMass());
    }
    _normalizer->init(maxMass);

    for (auto& gas : _gases) {
        gas.setMass(_normalizer->normalize(gas.getMass(), Normalizer::Type::MASS));
    }
    for (auto& betaChain : _betaChains) {
        betaChain.setLambda1(_normalizer->normalize(betaChain.getLambda1(), Normalizer::Type::LAMBDA));
        betaChain.setLambda2(_normalizer->normalize(betaChain.getLambda2(), Normalizer::Type::LAMBDA));
    }
    for (auto& initialParam : _initialParameters) {
        for (auto gi = 0; gi < _gases.size(); gi++) {
            initialParam.setPressure(gi, _normalizer->normalize(initialParam.getPressure(gi), Normalizer::Type::PRESSURE));
            initialParam.setTemperature(gi, _normalizer->normalize(initialParam.getTemperature(gi), Normalizer::Type::TEMPERATURE));
        }
    }
    for (auto& boundaryParam : _boundaryParameters) {
        for (auto gi = 0; gi < _gases.size(); gi++) {
            boundaryParam.setPressure(gi, _normalizer->normalize(boundaryParam.getPressure(gi), Normalizer::Type::PRESSURE));
            boundaryParam.setTemperature(gi, _normalizer->normalize(boundaryParam.getTemperature(gi), Normalizer::Type::TEMPERATURE));
        }
    }

    if (_impulseSphere == nullptr) {
        _impulseSphere.reset(new ImpulseSphere(4.8, 20));
    }
    _impulseSphere->init();
}

void Config::load(const std::string& filename) {
    boost::property_tree::ptree root;
    boost::property_tree::read_json(filename, root);

    _meshFilename = root.get<std::string>("mesh", "");
    _outputFolder = root.get<std::string>("output_folder", "./");
    _maxIterations = root.get<unsigned int>("max_iterations", 0);
    _outEachIteration = root.get<unsigned int>("out_each_iteration", 1);
    _isUsingIntegral = root.get<bool>("use_integral", false);
    _isUsingBetaDecay = root.get<bool>("use_beta_decay", false);

    _gases.clear();
    auto gasesNode = root.get_child_optional("gases");
    if (gasesNode) {
        for (const boost::property_tree::ptree::value_type& gas : *gasesNode) {
            auto mass = gas.second.get_value<double>();
            _gases.emplace_back(mass);
        }
    }

    _betaChains.clear();
    auto betaChainsNode = root.get_child_optional("beta_chains");
    if (betaChainsNode) {
        for (const boost::property_tree::ptree::value_type& betaChain : *betaChainsNode) {
            auto gi1 = betaChain.second.get<unsigned int>("gi1");
            auto gi2 = betaChain.second.get<unsigned int>("gi2");
            auto gi3 = betaChain.second.get<unsigned int>("gi3");
            auto lambda1 = betaChain.second.get<double>("lambda1");
            auto lambda2 = betaChain.second.get<double>("lambda2");
            _betaChains.emplace_back(gi1, gi2, gi3, lambda1, lambda2);
        }
    }

    _initialParameters.clear();
    auto initalNode = root.get_child_optional("initial");
    if (initalNode) {
        for (const boost::property_tree::ptree::value_type& param : *initalNode) {
            auto group = param.second.get<std::string>("group");

            std::vector<double> pressure;
            for (const boost::property_tree::ptree::value_type& value : param.second.get_child("pressure")) {
                pressure.emplace_back(value.second.get_value<double>());
            }
            pressure.resize(_gases.size(), 0.0);

            std::vector<double> temperature;
            for (const boost::property_tree::ptree::value_type& value : param.second.get_child("temperature")) {
                temperature.emplace_back(value.second.get_value<double>());
            }
            temperature.resize(_gases.size(), 0.0);

            _initialParameters.emplace_back(group, pressure, temperature);
        }
    }

    _boundaryParameters.clear();
    auto boundaryNode = root.get_child_optional("boundary");
    if (boundaryNode) {
        for (const boost::property_tree::ptree::value_type& param : *boundaryNode) {
            auto group = param.second.get<std::string>("group");

            std::vector<std::string> type;
            auto typeNode = param.second.get_child_optional("type");
            if (typeNode) {
                for (const boost::property_tree::ptree::value_type& value : *typeNode) {
                    type.emplace_back(value.second.get_value<std::string>());
                }
            }

            std::vector<double> pressure;
            auto pressureNode = param.second.get_child_optional("pressure");
            if (pressureNode) {
                for (const boost::property_tree::ptree::value_type& value : *pressureNode) {
                    pressure.emplace_back(value.second.get_value<double>());
                }
            }
            pressure.resize(_gases.size(), 0.0);

            std::vector<double> temperature;
            auto temperatureNode = param.second.get_child_optional("temperature");
            if (temperatureNode) {
                for (const boost::property_tree::ptree::value_type& value : *temperatureNode) {
                    temperature.emplace_back(value.second.get_value<double>());
                }
            }
            temperature.resize(_gases.size(), 0.0);

            _boundaryParameters.emplace_back(group, type, temperature, pressure);
        }
    }
}

const std::string& Config::getMeshFilename() const {
    return _meshFilename;
}

const std::string& Config::getOutputFolder() const {
    return _outputFolder;
}

unsigned int Config::getMaxIterations() const {
    return _maxIterations;
}

unsigned int Config::getOutEachIteration() const {
    return _outEachIteration;
}

bool Config::isUsingIntegral() const {
    return _isUsingIntegral;
}

bool Config::isUsingBetaDecay() const {
    return _isUsingBetaDecay;
}

const std::vector<Gas>& Config::getGases() const {
    return _gases;
}

const std::vector<BetaChain>& Config::getBetaChains() const {
    return _betaChains;
}

const std::vector<InitialParameters>& Config::getInitialParameters() const {
    return _initialParameters;
}

const std::vector<BoundaryParameters>& Config::getBoundaryParameters() const {
    return _boundaryParameters;
}

Normalizer* Config::getNormalizer() const {
    return _normalizer.get();
}

ImpulseSphere* Config::getImpulseSphere() const {
    return _impulseSphere.get();
}

void Config::setTimestep(double timestep) {
    _timestep = timestep;
}

double Config::getTimestep() const {
    return _timestep;
}

std::ostream& operator<<(std::ostream& os, const Config& config) {
    os << "MeshFilename = "     << config._meshFilename                        << std::endl
       << "OutputFolder = "     << config._outputFolder                        << std::endl
       << "MaxIteration = "     << config._maxIterations                       << std::endl
       << "OutEachIteration = " << config._outEachIteration                    << std::endl
       << "UseIntegral = "      << config._isUsingIntegral                     << std::endl
       << "UseBetaDecay = "     << config._isUsingBetaDecay                    << std::endl;

    os << "Gases = "            << Utils::toString(config._gases)              << std::endl;
    os << "BetaChains = "       << Utils::toString(config._betaChains)         << std::endl;
    os << "Initial = "          << Utils::toString(config._initialParameters)  << std::endl;
    os << "Boundary = "         << Utils::toString(config._boundaryParameters) << std::endl;

    os << "Normalizer = "       << *config._normalizer                         << std::endl
       << "ImpulseSphere = "          << *config._impulseSphere;
    return os;
}
