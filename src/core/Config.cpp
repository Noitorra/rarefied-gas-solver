#include "Config.h"

#include <sstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

Config* Config::_instance = nullptr;

Config::Config() : _normalizer(new Normalizer()), _impulseSphere(new ImpulseSphere(4.8, 20)) {
    _timestep = 0.0;
}

void Config::init() {
    _normalizer->init();
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

            std::vector<double> temperature;
            for (const boost::property_tree::ptree::value_type& value : param.second.get_child("temperature")) {
                temperature.emplace_back(value.second.get_value<double>());
            }

            _initialParameters.emplace_back(group, pressure, temperature);
        }
    }

    _boundaryParameters.clear();
    auto boundaryNode = root.get_child_optional("boundary");
    if (boundaryNode) {
        for (const boost::property_tree::ptree::value_type& param : *boundaryNode) {
            auto group = param.second.get<std::string>("group");
            auto type = param.second.get<std::string>("type");

            std::vector<double> temperature;
            auto temperatureNode = param.second.get_child_optional("temperature");
            if (temperatureNode) {
                for (const boost::property_tree::ptree::value_type& value : *temperatureNode) {
                    temperature.emplace_back(value.second.get_value<double>());
                }
            }

            _boundaryParameters.emplace_back(group, type, temperature);
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
