#include "Config.h"

#include <sstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

Config* Config::_instance = nullptr;

void Config::init() {
    if (_normalizer == nullptr) {
        _normalizer.reset(new Normalizer());
    }

    double maxMass = 0.0, maxRadius = 0.0;
    for (const auto& gas : _gases) {
        maxMass = std::max(maxMass, gas.getMass());
        maxRadius = std::max(maxRadius, gas.getRadius());
    }
    double maxPressure = 0.0, maxTemperature = 0.0;
    for (auto& param : _initialParameters) {
        for (auto gi = 0; gi < _gases.size(); gi++) {
            maxPressure = std::max(maxPressure, param.getPressure(gi));
            maxTemperature = std::max(maxTemperature, param.getTemperature(gi));
            if (param.hasGradientTemperature(gi)) {
                maxTemperature = std::max(maxTemperature, param.getGradientTemperature(gi).getValueStart());
                maxTemperature = std::max(maxTemperature, param.getGradientTemperature(gi).getValueEnd());
            }
        }
    }
    _normalizer->init(maxMass, maxRadius, maxPressure, maxTemperature);

    for (auto& gas : _gases) {
        gas.setMass(_normalizer->normalize(gas.getMass(), Normalizer::Type::MASS));
        gas.setRadius(_normalizer->normalize(gas.getRadius(), Normalizer::Type::RADIUS));
    }
    for (auto& betaChain : _betaChains) {
        betaChain.setLambda1(_normalizer->normalize(betaChain.getLambda1(), Normalizer::Type::LAMBDA));
        betaChain.setLambda2(_normalizer->normalize(betaChain.getLambda2(), Normalizer::Type::LAMBDA));
    }
    for (auto& param : _initialParameters) {
        for (auto gi = 0; gi < _gases.size(); gi++) {
            param.setPressure(gi, _normalizer->normalize(param.getPressure(gi), Normalizer::Type::PRESSURE));
            param.setTemperature(gi, _normalizer->normalize(param.getTemperature(gi), Normalizer::Type::TEMPERATURE));

            if (param.hasGradientTemperature(gi)) {
                GradientParameter gradientTemperature = param.getGradientTemperature(gi);
                gradientTemperature.setValueStart(_normalizer->normalize(gradientTemperature.getValueStart(), Normalizer::Type::TEMPERATURE));
                gradientTemperature.setValueEnd(_normalizer->normalize(gradientTemperature.getValueEnd(), Normalizer::Type::TEMPERATURE));

                Vector3d pointStart = gradientTemperature.getPointStart();
                pointStart.x() = pointStart.x() * _meshUnits;
                pointStart.y() = pointStart.y() * _meshUnits;
                pointStart.z() = pointStart.z() * _meshUnits;
                gradientTemperature.setPointStart(pointStart);

                Vector3d pointEnd = gradientTemperature.getPointEnd();
                pointEnd.x() = pointEnd.x() * _meshUnits;
                pointEnd.y() = pointEnd.y() * _meshUnits;
                pointEnd.z() = pointEnd.z() * _meshUnits;
                gradientTemperature.setPointEnd(pointEnd);

                param.setGradientTemperature(gi, gradientTemperature);
            }

            if (param.hasGradientPressure(gi)) {
                GradientParameter gradientPressure = param.getGradientPressure(gi);
                gradientPressure.setValueStart(_normalizer->normalize(gradientPressure.getValueStart(), Normalizer::Type::PRESSURE));
                gradientPressure.setValueEnd(_normalizer->normalize(gradientPressure.getValueEnd(), Normalizer::Type::PRESSURE));

                Vector3d pointStart = gradientPressure.getPointStart();
                pointStart.x() = pointStart.x() * _meshUnits;
                pointStart.y() = pointStart.y() * _meshUnits;
                pointStart.z() = pointStart.z() * _meshUnits;
                gradientPressure.setPointStart(pointStart);

                Vector3d pointEnd = gradientPressure.getPointEnd();
                pointEnd.x() = pointEnd.x() * _meshUnits;
                pointEnd.y() = pointEnd.y() * _meshUnits;
                pointEnd.z() = pointEnd.z() * _meshUnits;
                gradientPressure.setPointEnd(pointEnd);

                param.setGradientPressure(gi, gradientPressure);
            }
        }
    }
    for (auto& param : _boundaryParameters) {
        for (auto gi = 0; gi < _gases.size(); gi++) {
            param.setPressure(gi, _normalizer->normalize(param.getPressure(gi), Normalizer::Type::PRESSURE));
            param.setTemperature(gi, _normalizer->normalize(param.getTemperature(gi), Normalizer::Type::TEMPERATURE));

            double flow = param.getFlow(gi);
            _normalizer->normalize(flow, Normalizer::Type::FLOW);
            param.setFlow(gi, flow);

            if (param.hasGradientTemperature(gi)) {
                GradientParameter gradientTemperature = param.getGradientTemperature(gi);
                gradientTemperature.setValueStart(_normalizer->normalize(gradientTemperature.getValueStart(), Normalizer::Type::TEMPERATURE));
                gradientTemperature.setValueEnd(_normalizer->normalize(gradientTemperature.getValueEnd(), Normalizer::Type::TEMPERATURE));

                Vector3d pointStart = gradientTemperature.getPointStart();
                pointStart.x() = pointStart.x() * _meshUnits;
                pointStart.y() = pointStart.y() * _meshUnits;
                pointStart.z() = pointStart.z() * _meshUnits;
                gradientTemperature.setPointStart(pointStart);

                Vector3d pointEnd = gradientTemperature.getPointEnd();
                pointEnd.x() = pointEnd.x() * _meshUnits;
                pointEnd.y() = pointEnd.y() * _meshUnits;
                pointEnd.z() = pointEnd.z() * _meshUnits;
                gradientTemperature.setPointEnd(pointEnd);

                param.setGradientTemperature(gi, gradientTemperature);
            }
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
    _meshUnits = root.get<double>("mesh_units", 1.0);

    _outputFolder = root.get<std::string>("output_folder", "./");
    _maxIterations = root.get<unsigned int>("max_iterations", 0);
    _outEachIteration = root.get<unsigned int>("out_each_iteration", 1);
    _isUsingIntegral = root.get<bool>("use_integral", false);
    _isUsingBetaDecay = root.get<bool>("use_beta_decay", false);
    _isImplicitScheme = root.get<bool>("use_implicit_scheme", false);

    _gases.clear();
    auto gasesNode = root.get_child_optional("gases");
    if (gasesNode) {
        for (const boost::property_tree::ptree::value_type& gas : *gasesNode) {
            auto mass = gas.second.get<double>("mass") * 1.66e-27; // aem to kg
            auto radius = gas.second.get<double>("radius") * 1e-12; // rad
            _gases.emplace_back(mass, radius);
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

            std::vector<GradientParameter> gradientTemperature;
            auto gradientTemperatureNode = param.second.get_child_optional("temperature_gradient");
            if (gradientTemperatureNode) {
                for (const boost::property_tree::ptree::value_type& value : *gradientTemperatureNode) {
                    double valueStart = value.second.get<double>("value_start", 0);
                    double valueEnd = value.second.get<double>("value_end", 0);

                    Vector3d pointStart(0, 0, 0);
                    auto pointStartNode = value.second.get_child_optional("point_start");
                    if (pointStartNode) {
                        double x = pointStartNode->get<double>("x", 0);
                        double y = pointStartNode->get<double>("y", 0);
                        double z = pointStartNode->get<double>("z", 0);
                        pointStart.set(x, y, z);
                    }

                    Vector3d pointEnd(0, 0, 0);
                    auto pointEndNode = value.second.get_child_optional("point_end");
                    if (pointEndNode) {
                        double x = pointEndNode->get<double>("x", 0);
                        double y = pointEndNode->get<double>("y", 0);
                        double z = pointEndNode->get<double>("z", 0);
                        pointEnd.set(x, y, z);
                    }

                    gradientTemperature.emplace_back(valueStart, valueEnd, pointStart, pointEnd);
                }
            }

            std::vector<GradientParameter> gradientPressure;
            auto gradientPressureNode = param.second.get_child_optional("pressure_gradient");
            if (gradientPressureNode) {
                for (const boost::property_tree::ptree::value_type& value : *gradientPressureNode) {
                    double valueStart = value.second.get<double>("value_start", 0);
                    double valueEnd = value.second.get<double>("value_end", 0);

                    Vector3d pointStart(0, 0, 0);
                    auto pointStartNode = value.second.get_child_optional("point_start");
                    if (pointStartNode) {
                        double x = pointStartNode->get<double>("x", 0);
                        double y = pointStartNode->get<double>("y", 0);
                        double z = pointStartNode->get<double>("z", 0);
                        pointStart.set(x, y, z);
                    }

                    Vector3d pointEnd(0, 0, 0);
                    auto pointEndNode = value.second.get_child_optional("point_end");
                    if (pointEndNode) {
                        double x = pointEndNode->get<double>("x", 0);
                        double y = pointEndNode->get<double>("y", 0);
                        double z = pointEndNode->get<double>("z", 0);
                        pointEnd.set(x, y, z);
                    }

                    gradientPressure.emplace_back(valueStart, valueEnd, pointStart, pointEnd);
                }
            }

            _initialParameters.emplace_back(group, pressure, temperature, gradientTemperature, gradientPressure);
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

            std::vector<double> flow;
            auto flowNode = param.second.get_child_optional("flow");
            if (flowNode) {
                for (const boost::property_tree::ptree::value_type& value : *flowNode) {
                    flow.emplace_back(value.second.get_value<double>());
                }
            }
            flow.resize(_gases.size(), 0.0);

            std::vector<GradientParameter> gradientTemperature;
            auto gradientTemperatureNode = param.second.get_child_optional("temperature_gradient");
            if (gradientTemperatureNode) {
                for (const boost::property_tree::ptree::value_type& value : *gradientTemperatureNode) {
                    double valueStart = value.second.get<double>("value_start", 0);
                    double valueEnd = value.second.get<double>("value_end", 0);

                    Vector3d pointStart(0, 0, 0);
                    auto pointStartNode = value.second.get_child_optional("point_start");
                    if (pointStartNode) {
                        double x = pointStartNode->get<double>("x", 0);
                        double y = pointStartNode->get<double>("y", 0);
                        double z = pointStartNode->get<double>("z", 0);
                        pointStart.set(x, y, z);
                    }

                    Vector3d pointEnd(0, 0, 0);
                    auto pointEndNode = value.second.get_child_optional("point_end");
                    if (pointEndNode) {
                        double x = pointEndNode->get<double>("x", 0);
                        double y = pointEndNode->get<double>("y", 0);
                        double z = pointEndNode->get<double>("z", 0);
                        pointEnd.set(x, y, z);
                    }

                    gradientTemperature.emplace_back(valueStart, valueEnd, pointStart, pointEnd);
                }
            }

            auto groupConnect = param.second.get<std::string>("group_connect", "");

            _boundaryParameters.emplace_back(group, type, temperature, pressure, flow, gradientTemperature, groupConnect);
        }
    }
}

std::ostream& operator<<(std::ostream& os, const Config& config) {
    os << "mesh_filename = "      << config._meshFilename                        << std::endl
       << "output_folder = "      << config._outputFolder                        << std::endl
       << "max_iteration = "      << config._maxIterations                       << std::endl
       << "out_each_iteration = " << config._outEachIteration                    << std::endl
       << "use_integral = "       << config._isUsingIntegral                     << std::endl
       << "use_beta_decay = "     << config._isUsingBetaDecay                    << std::endl;

    os << "gases = "              << Utils::toString(config._gases)              << std::endl;
    os << "beta_chains = "        << Utils::toString(config._betaChains)         << std::endl;
    os << "initial params = "     << Utils::toString(config._initialParameters)  << std::endl;
    os << "boundary params = "    << Utils::toString(config._boundaryParameters) << std::endl;

    os << "normalizer = "         << *config._normalizer                         << std::endl
       << "impulse_phere = "      << *config._impulseSphere;
    return os;
}
