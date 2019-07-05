#ifndef RGS_UTILS_H
#define RGS_UTILS_H

#include <sstream>
#include <vector>
#include <chrono>

const double BOLTZMANN_CONSTANT = 1.38e-23; // Boltzmann const // TODO: Make more precise

class Utils {
public:
    template<typename T>
    static inline int sgn(const T& val) {
        return (T(0) < val) - (val < T(0));
    }

    template<typename T>
    static std::string toString(const T& value) {
        std::stringstream ss;
        ss << value;
        return ss.str();
    }

    template<typename T>
    static std::string toString(const std::vector<T>& vector) {
        std::stringstream ss;
        ss << "[";
        for (unsigned int i = 0; i < vector.size(); i++) {
            ss << vector[i];
            if (i != vector.size() - 1) {
                ss << ", ";
            }
        }
        ss << "]";
        std::string str = ss.str();
        if (str.length() > 100) {
            ss.str("");
            ss << "[";
            for (unsigned int i = 0; i < vector.size(); i++) {
                if (i == 0) {
                    ss << std::endl;
                }
                ss << "    " << vector[i];
                if (i != vector.size() - 1) {
                    ss << ", ";
                }
                ss << std::endl;
            }
            ss << "]";
            str = ss.str();
        }
        return str;
    }

    template<typename Enumeration>
    static inline int asNumber(const Enumeration value) {
        return static_cast<int>(value);
    }

    template<typename Enumeration>
    static inline std::vector<int> asNumberVector(const std::vector<Enumeration>& values) {
        std::vector<int> basicValues;
        for (unsigned int i = 0; i < values.size(); i++) {
            basicValues.push_back(static_cast<int>(values[i]));
        }
        return basicValues;
    }

    static std::string getCurrentDateAndTime() {
        auto now = std::chrono::system_clock::now();
        auto now_time_t = std::chrono::system_clock::to_time_t(now);
        char buffer[100];
        if (std::strftime(buffer, sizeof(buffer), "%Y-%m-%d_%H-%M-%S", std::localtime(&now_time_t)) > 0) {
            return std::string(buffer);
        } else {
            return "unknown";
        }
    }

};


#endif //RGS_UTILS_H
