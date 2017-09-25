#ifndef RGS_UTILS_H
#define RGS_UTILS_H

#include <sstream>
#include <vector>
#include <chrono>

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>

const double BOLTZMANN_CONSTANT = 1.38e-23; // Boltzmann const // TODO: Make more precise

class Utils {
public:
    template<typename T>
    static inline int sgn(T val) {
        return (T(0) < val) - (val < T(0));
    }

    template<typename T>
    static std::string toString(T const &value) {
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
    static inline auto asNumber(Enumeration const value) -> typename std::underlying_type<Enumeration>::type {
        return static_cast<typename std::underlying_type<Enumeration>::type>(value);
    }

    template<class T>
    static std::string serialize(T object) {
        std::ostringstream os;
        boost::archive::binary_oarchive oa(os);
        oa << object;
        return os.str();
    }

    template<class T>
    static void deserialize(const std::string& buffer, T& object) {
        std::istringstream is(buffer);
        boost::archive::binary_iarchive ia(is);
        ia >> object;
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
