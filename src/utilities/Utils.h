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
            if (i != 0) {
                ss << ", ";
            }
            ss << vector[i];
        }
        ss << "]";
        return ss.str();
    }

    template<typename Enumeration>
    static inline auto asNumber(const Enumeration value) -> typename std::underlying_type<Enumeration>::type {
        return static_cast<typename std::underlying_type<Enumeration>::type>(value);
    }

    template<typename Enumeration>
    static inline auto asNumberVector(const std::vector<Enumeration>& values) -> std::vector<typename std::underlying_type<Enumeration>::type> {
        std::vector<typename std::underlying_type<Enumeration>::type> basicValues;
        for (unsigned int i = 0; i < values.size(); i++) {
            basicValues.push_back(static_cast<typename std::underlying_type<Enumeration>::type>(values[i]));
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
